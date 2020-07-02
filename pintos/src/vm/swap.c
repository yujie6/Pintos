//
// Created by yujie6 on 6/4/20.
//

#include "swap.h"



static struct lock swap_lock;
static struct bitmap *swap_map;
static struct block *swap_block;

void swap_init() {
    size_t swap_size;
    lock_init(&swap_lock);
    swap_block = block_get_role(BLOCK_SWAP);
    swap_size = block_size(swap_block) / SECTORS_PER_PAGE;
    swap_map = bitmap_create(swap_size);
    bitmap_set_all(swap_map, true);
}




swap_index_t swap_out(void *page) {
    lock_acquire (&swap_lock);
    size_t free_index = bitmap_scan_and_flip (swap_map, 0, 1, true);
    if (free_index == BITMAP_ERROR) {
        PANIC ("Swap device is full");
    }
    else {
        for (size_t i = 0; i < SECTORS_PER_PAGE; ++i){
            block_write (swap_block, free_index * SECTORS_PER_PAGE + i, page + BLOCK_SECTOR_SIZE * i);
        }
        bitmap_set(swap_map, free_index, false);
        lock_release (&swap_lock);  
        return free_index;
    }
}

void swap_in(swap_index_t swap_index, void *page) {
    lock_acquire (&swap_lock);
    if (bitmap_test (swap_map, swap_index) == 0) {
        
        for (size_t i = 0; i < SECTORS_PER_PAGE; i++)
            block_read (swap_block, swap_index * SECTORS_PER_PAGE + i, page + BLOCK_SECTOR_SIZE * i);
        bitmap_set (swap_map, swap_index, true);
        lock_release(&swap_lock);
    }
    else {
        PANIC ("Swap block is free");
    }
}


void swap_free (swap_index_t swap_index) {

  if (bitmap_test(swap_map, swap_index) == true) {
    PANIC ("Error");
  }
  bitmap_set(swap_map, swap_index, true);
}
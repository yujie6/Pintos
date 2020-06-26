//
// Created by yujie6 on 6/4/20.
//

#ifndef SRC_SWAP_H
#define SRC_SWAP_H


#include <bitmap.h>
#include "threads/vaddr.h"
#include "devices/block.h"

typedef uint32_t swap_index_t;

//init the swap_table

static const size_t SECTORS_PER_PAGE = PGSIZE / BLOCK_SECTOR_SIZE;

struct lock swap_lock;
struct bitmap *swap_map;
struct block *swap_block_device;

void swap_init();

/*
 * Swap Out: write the content of `page` into the swap disk,
 * and return the index of swap region in which it is placed.
 */

swap_index_t swap_out(void *page);

/**
 * Swap In: read the content of from the specified swap index,
 * from the mapped swap block, and store PGSIZE bytes into `page`.
 */

void swap_in(swap_index_t swap_index,void *page);

/**
 * Free Swap: drop the swap region.
 */
void swap_free(swap_index_t swap_index);

#endif //SRC_SWAP_H

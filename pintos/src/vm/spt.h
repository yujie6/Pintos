// created by wuhuaijin

#ifndef SRC_SPT_H
#define SRC_SPT_H

#include <lib/kernel/hash.h>
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "swap.h"
#include "userprog/pagedir.h"

enum page_status {
    ALL_ZERO,
    FRAME, // in frame
    FILE, // in file system
    SWAP // in swap slot
};


struct page_table_item{
    enum page_status status;
    // virtural address of page;
    void *key; 
    //physical address of page, if page is not on the frame should be null
    void *value;
    

    bool dirty;

    //for SWAP pages
    swap_index_t swap_index;

    //for FILE
    bool writable;
    struct file *file;
    off_t file_off;
    uint32_t read_bytes, zero_bytes;
    struct hash_elem hash_elem;
};


struct hash * spt_create(void);
// void spt_init(struct hash *spt);
void spt_destroy(struct hash *spt);
struct page_table_item * find_page(struct hash *spt, void *page);
bool spt_has_item(struct hash *spt, void *page);
bool spt_set_dirty(struct hash *spt, void *page, bool dirty);
bool load_page(struct hash *spt, uint32_t *pagedir, void *upage);

bool spt_unmap(struct hash *spt, uint32_t *pagedir, void *upage, struct file *file, off_t offset, size_t bytes);


bool spt_install_frame(struct hash *spt, void *upage, void *kpage);
bool spt_install_zeropage(struct hash *spt, void *page);
bool spt_set_swap(struct hash *spt, void *page, swap_index_t swap_index);
bool spt_install_file(struct hash *spt, void *page, struct file *file, off_t offset,
                        uint32_t read_bytes, uint32_t zero_bytes, bool writable);



#endif
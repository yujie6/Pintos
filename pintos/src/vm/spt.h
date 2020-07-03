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


struct s_page_table {
    struct hash page_map;
};


struct page_table_item {
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

struct mmap_info {
    // store mmap_id
    struct list_elem elem;
    int id;
    struct file * file;
    int size;
    void * addr;
};


struct s_page_table * spt_init(void);
// void spt_init(struct hash *spt);
void spt_destroy(struct s_page_table *spt);
struct page_table_item * find_page(struct s_page_table *spt, void *page);
bool spt_has_item(struct s_page_table *spt, void *page);
bool spt_set_dirty(struct s_page_table *spt, void *page, bool dirty);
bool load_page(struct s_page_table *spt, uint32_t *pagedir, void *upage);

bool spt_unmap(struct s_page_table *spt, uint32_t *pagedir, void *upage, struct file *file, off_t offset, size_t bytes);


bool spt_install_frame(struct s_page_table *spt, void *upage, void *kpage);
bool spt_install_zeropage(struct s_page_table *spt, void *page);
bool spt_set_swap(struct s_page_table *spt, void *page, swap_index_t swap_index);
bool spt_install_file(struct s_page_table *spt, void *page, struct file *file, off_t offset,
                        uint32_t read_bytes, uint32_t zero_bytes, bool writable);



#endif
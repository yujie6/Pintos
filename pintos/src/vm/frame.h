#ifndef SRC_FRAME_H
#define SRC_FRAME_H

#include <hash.h>
#include <list.h>
#include <debug.h>
#include "threads/palloc.h"
#include "threads/synch.h"

struct frame_item{
    void *frame;
    struct thread *thread_use;
    void *page;
    struct list_elem list_elem;
    struct hash_elem hash_elem;
    bool pinned;
};

struct hash frame_table;

struct lock frame_lock;

//for clock algorithm
struct list frame_clock_list;
struct frame_item *frame_ptr;

void frame_table_init();

// get a frame, then in page talbe page->get_frame*
void* get_frame(void *page, enum palloc_flags flag);

//free a frame
void free_frame(void *frame);

void *evict_frame(enum palloc_flags flag);

//get whether a frame can be swap
bool get_pin_info(void *frame);

//set a frame to be pinned, return if successful
bool set_pin_info(void *frame, bool value);

//help to find the frame
void *find_frame(void *frame);


#endif
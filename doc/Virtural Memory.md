# Project 3 Design Document

> SJTU MS110
>
> Spring 2020

## GROUP

+ Yujie Lu

+ Huaijin Wu

+ Tong Chen

## PROJECT PARTS

#### A.PAGE TABLE MANAGMENT

##### DATA STURCTURES

```c++
//spt.h
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
//frame.h
struct frame_item{
    void *frame;
    struct thread *thread_use;
    void *page;
    struct list_elem list_elem;
    struct hash_elem hash_elem;
    bool pinned;
};
```

#### B.SWAP SLOT

##### DATA STURCTURE

```c++
static struct lock swap_lock;
static struct bitmap *swap_map;
static struct block *swap_block;
```

##### ALGOTITHMS

We apply clock algorithm. 

#### C.MEMORY MAPPED FILES

##### DATA STRUCTURE

```c++
struct mmap_info {
    // store mmap_id
    struct list_elem elem;
    int id;
    struct file * file;
    int size;
    void * addr;
};
```






#include "frame.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "spt.h"
#include "swap.h"
#include "userprog/pagedir.h"
#include "threads/malloc.h"
#include "list.h"


// reconstruct
static bool frame_hash_less(const struct hash_elem *x, const struct hash_elem *y, void *aux UNUSED) {
    const struct frame_item * xx = hash_entry(x, struct frame_item, hash_elem);
    const struct frame_item * yy = hash_entry(y, struct frame_item, hash_elem);
    return xx->frame < yy->frame;
}

//reconstruct
static bool frame_hash(const struct hash_elem *x, void *aux UNUSED) {
    const struct frame_item *xx = hash_entry(x, struct frame_item, hash_elem);
    return hash_bytes(&xx->frame, sizeof(xx->frame));
}


//method
void *find_frame(void *frame) {
    struct frame_item p;
    struct hash_elem *q;
    p.frame = frame;
    q = hash_find(&frame_table, &p.hash_elem);
    if (q == NULL) return NULL;
    return hash_entry(q, struct frame_item, hash_elem);
}

void frame_init() {
    frame_ptr = NULL;
    lock_init(&frame_lock);
    hash_init(&frame_table, frame_hash, frame_hash_less, NULL);
    list_init(&frame_clock_list);
}

void * get_frame(void *page, enum palloc_flags flag) {

    lock_acquire(&frame_lock);

    void *frame_addr = palloc_get_page(PAL_USER | flag);

    if (frame_addr == NULL) {
        frame_addr = evict_frame(PAL_USER | flag);
    }

    if (frame_addr == NULL) {
        PANIC ("no frame");
        lock_release(&frame_lock);
        return frame_addr;
    } 

    if (frame_addr != NULL) {
        struct frame_item *t = malloc (sizeof (struct frame_item));
        t->page = page;
        t->frame = frame_addr;
        t->thread_use = thread_current();
        t->pinned = true;
        hash_insert(&frame_table, &t->hash_elem);
        list_push_back(&frame_clock_list, &t->list_elem);
        lock_release(&frame_table);
    }

    return frame_addr;

}

void free_frame(void *frame) {
    if (frame == NULL) return;
    lock_acquire(&frame_lock);

    struct frame_item *t = find_frame(frame);

    if (t != NULL) {
        hash_delete(&frame_table, &t->hash_elem);
        list_remove(&t->list_elem);
        palloc_free_page (frame);
        free(t);
        lock_release (&frame_lock);
    }
    else {
        PANIC ("frame not exist");
    }

}

//clock algorithm

void clock_frame_next(void);

void *frame_evict(enum palloc_flags flag, uint32_t *pagedir) {
    size_t n = hash_size(&frame_table);

    for (size_t i = 0; i <= 2*n; ++i) {
        clock_frame_next();
        if (frame_ptr->pinned) continue;

        if (pagedir_is_accessed(pagedir, frame_ptr->frame)) {
            pagedir_set_accessed(pagedir, frame_ptr->frame, false);
            continue;
        }

        pagedir_clear_page(frame_ptr->thread_use->pagedir, frame_ptr->frame);

        bool is_dirty = false;
        is_dirty = is_dirty || pagedir_is_dirty(frame_ptr->thread_use->pagedir, frame_ptr->frame);
        is_dirty = is_dirty || pagedir_is_dirty(frame_ptr->thread_use->pagedir, frame_ptr->page);

        swap_index_t swap_idx = swap_out(frame_ptr->page);
        spt_set_swap(frame_ptr->thread_use->pagedir, frame_ptr->frame, swap_idx);
        spt_set_dirty(frame_ptr->thread_use->pagedir, frame_ptr->frame, is_dirty);
        free_frame(frame_ptr->page); // f_evicted is also invalidated

        return palloc_get_page (flag);
    }
    PANIC ("no space");
}

void clock_frame_next(void) {
    if (list_size(&frame_clock_list) == 1)
        return;
    if (&frame_ptr->list_elem == list_back(&frame_clock_list) || frame_ptr == NULL)
        frame_ptr = list_entry(list_head(&frame_clock_list), struct frame_item, list_elem);
    frame_ptr = list_entry(list_next(&frame_clock_list), struct frame_item, list_elem);                            
}




bool get_pin_info(void *frame) {
    struct frame_item *t = find_frame(frame);
    if (t != NULL) {
        return t->pinned;
    }
    else {
        PANIC ("frame not exist");
    }
}

bool set_pin_info(void *frame, bool value) {
    lock_acquire(&frame_lock);
    struct frame_item *t = find_frame(frame);
    if (t != NULL) {
        t->pinned = value;
        lock_release(&frame_lock);
        return true;
    }
    else {
        lock_release(&frame_lock);
        return false;
    }
}

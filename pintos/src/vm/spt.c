#include <threads/malloc.h>
#include "spt.h"
#include "frame.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "swap.h"
#include "userprog/pagedir.h"
#include "lib/kernel/hash.h"
#include <string.h>

static void spt_hash_clear(struct hash_elem *e, void *aux UNUSED);
static unsigned spt_hash(const struct hash_elem *e, void *aux UNUSED);
static bool spt_hash_less(const struct hash_elem *x, const struct hash_elem *y, void *aux UNUSED);

static unsigned spt_hash(const struct hash_elem *e, void *aux UNUSED) {
    const struct page_table_item * t = hash_entry(e, struct page_table_item, hash_elem);
	return hash_int( (int)t->key);
}

static bool spt_hash_less(const struct hash_elem *x, const struct hash_elem *y, void *aux UNUSED) {
    struct page_table_item *xx = hash_entry(x, struct page_table_item, hash_elem);
    struct page_table_item *yy = hash_entry(y, struct page_table_item, hash_elem);
    return xx->key < yy->key;
}

static void spt_hash_clear(struct hash_elem *e, void *aux UNUSED) {
    struct page_table_item *t = hash_entry(e, struct page_table_item, hash_elem);
    if(t->status == FRAME) {
	    pagedir_clear_page(thread_current()->pagedir, t->key);
	    free_frame(t->value);
	}
    else if (t->status == SWAP) {
        swap_free(t->swap_index);
    }
	free(t);
}

struct hash * spt_create(void) {
    struct hash *spt = malloc(sizeof(struct hash));
    hash_init(spt, spt_hash, spt_hash_less, NULL);
    return spt;
}


void spt_destroy(struct hash *spt) {
    hash_destroy (spt, spt_hash_clear);
}

struct page_table_item * find_page(struct hash *spt, void *page) {
    struct hash_elem *e;
	struct page_table_item tmp;
	tmp.key = page;
	e = hash_find(spt, &(tmp.hash_elem));

	if(e != NULL) {
		return hash_entry(e, struct page_table_item, hash_elem);
	}
	else {
		return NULL;
	}
}

bool spt_has_item(struct hash *spt, void *page) {
    struct page_table_item *e = find_page(spt, page);
    if (e == NULL) return false;
    return true;
}

bool spt_set_dirty(struct hash *spt, void *page, bool dirty) {
    struct page_table_item *e = find_page(spt, page);
    if (e == NULL) PANIC("page not exist when set dirty");
    e->dirty = e->dirty || dirty;
    return true;
}

bool load_page(struct hash *spt, uint32_t *pagedir, void *upage) {
    struct page_table_item *e = find_page(spt, upage);
    if (e == NULL) return false;

    if (e->status == FRAME) {
        return true; // already in frame, so don't need to load again
    }

    void *frame = get_frame(upage, PAL_USER);

    if (frame == NULL) return false;
    bool writable = false;

    if(e->status == ALL_ZERO) {
        memset(frame, 0, PGSIZE);
    }
    else if (e->status == SWAP) {
        swap_in(e->swap_index, frame);
    }
    else if (e->status == FILE) {
        file_seek(e->file, e->file_off);
        int n_read = file_read(e->file, upage, e->read_bytes);
        if (n_read != e->read_bytes) {
            free_frame(frame);
            return false;
        }
        ASSERT(e->read_bytes + e->zero_bytes == PGSIZE);
        memset(upage + n_read, 0, e->zero_bytes);
        writable = e->writable;
    }

    if (!pagedir_set_page(pagedir, upage, frame, writable)) {
        free_frame(frame);
        return false;
    }

    e->status = FRAME;
    e->value = frame;

    pagedir_set_dirty(pagedir, frame, false);

    set_pin_info(frame, false);

    return true;
}

bool spt_unmap(struct hash *spt, uint32_t *pagedir, void *upage, struct file *file, off_t offset, size_t bytes) {
    struct page_table_item *e = find_page(spt, upage);
    if (e == NULL) PANIC("page not exist when unmap");

    if (e->status == FRAME) {
        set_pin_info(e->value, true);
    }

    if (e->status == FRAME) {
        bool ifdirty = e->dirty;
        ifdirty = ifdirty || pagedir_is_dirty(pagedir, e->key);
        ifdirty = ifdirty || pagedir_is_dirty(pagedir, e->value);
        if (ifdirty) {
            file_write_at (file, e->key, bytes, offset);
        }
        free_frame(e->value);
        pagedir_clear_page(pagedir, e->key);
    }
    else if (e->status == SWAP) {
        bool ifdirty = e->dirty;
        ifdirty = ifdirty || pagedir_is_dirty(pagedir, e->key);
        if (ifdirty) {
            void *tmp_page = palloc_get_page(0);
            swap_in(e->swap_index, tmp_page);
            file_write_at (file, tmp_page, PGSIZE, offset);
            palloc_free_page(tmp_page);
        }
        else {
            swap_free(e->swap_index);
        }
    }

    hash_delete(spt, &e->hash_elem);
    return true;
}


bool spt_install_file(struct hash *spt, void *page, struct file *file, off_t offset,
                        uint32_t read_bytes, uint32_t zero_bytes, bool writable) {

    struct page_table_item *e = malloc(sizeof(struct page_table_item));
    e->key = page;
    e->read_bytes = read_bytes;
    e->zero_bytes = zero_bytes;
    e->writable = writable;
    e->file = file;
    e->file_off = offset;
    e->status = FILE;
    e->dirty = false;
    e->value = NULL;

    return hash_insert(spt, &e->hash_elem) == NULL;
}

//use in process.c install_page
bool spt_install_frame(struct hash *spt, void *upage, void *kpage) {
    struct page_table_elem *t = find_page(spt, upage);
    if (t != NULL) {
        return false;
    }
    struct page_table_item *e = malloc(sizeof(struct page_table_item));
    e->key = upage;
    e->value = kpage;
    e->status = FRAME;
    e->dirty = false;
    e->swap_index = -1;
    hash_insert(spt, &e->hash_elem);
    return true;
}

//use in exception.c page_fault
bool spt_install_zeropage(struct hash *spt, void *page) {
    struct page_table_item *e = malloc(sizeof(struct page_table_item));
    e->key = page;
    e->status = ALL_ZERO;
    e->dirty = false;
    e->value = NULL;

    return hash_insert(spt, &e->hash_elem) == NULL;
}

bool spt_set_swap(struct hash *spt, void *page, swap_index_t swap_index) {
    struct page_table_item *e = find_page(spt, page);
    if (e == NULL)  return false;
    e->status = SWAP;
    e->value = page;
    e->swap_index  = swap_index;
    return true;
}

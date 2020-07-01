#include "spt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "swap.h"
#include "lib/kernel/hash.h"
#include "userprog/pagedir.h"
#include "page.h"


static unsigned spt_hash(const struct hash_elem *e, void *aux UNUSED) {
    const struct page_table_item * t = hash_entry(e, struct page_table_item, hash_elem);
	return hash_bytes(&(t->key), sizeof(t->key));
}
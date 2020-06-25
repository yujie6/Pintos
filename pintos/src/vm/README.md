# Virtual Memory Notes
Now, a page fault might only indicate that the page must be 
brought in from a file or swap. should implement by modifying 
`page_fault()` in `userprog/exception.c`, needs to do
 the following
 
## 1. Structure Overview

* Supplemental page table
* Frame table
* Swap table
* Table of file mappings

it may be convenient to wholly or partially merge related 
resources into a unified data structure.

We may use 
* bitmap
* hashmap

# 2. Managing the Supplemental Page Table (SPT)
The supplemental page table supplements the page table 
with additional data about each page. It is needed 
because of the limitations imposed by the page table's format.

There are mainly 2 usages:
* on a page fault, the kernel looks up the virtual page that faulted 
in the supplemental page table to find out what data should be there
* kernel consults the supplemental page table when a process terminates
, to decide what resources to free

you may use the page table itself as an index to track the 
members of the supplemental page table. You will have to modify 
the Pintos page table implementation in `pagedir.c` to do so.

# 
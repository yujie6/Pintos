# Userprog 

## 1. Set up stack
Seting up stack is very tricky, we just follow the guidance on the official document.
And it works find after some debuging.

## 2. System call
In system call, first we implement some functions on file manipulation. Such as open, close,  write, read,
mainly based on the api given by `file.h`.

Second we implement `exec` and `wait` based on the function we implement in `process,c`.

Finnaly there are functions for vm, `mmap` and `munmap` which is also kind of trivial after 
reading the official documents.

set(userprog_SRCS
        gdt.c
        gdt.h
        process.c
        process.h
        exception.c
        exception.h
        syscall.c
        syscall.h
        pagedir.c
        pagedir.h
        tss.c
        tss.h
        )

add_library(userprog ${userprog_SRCS})
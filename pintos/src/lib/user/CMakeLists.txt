set(libsUser_SRCS
        console.c
        debug.c
        stdio.h
        entry.c
        syscall.c
        )

add_library(libUser ${libsUser_SRCS})
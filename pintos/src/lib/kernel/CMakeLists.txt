set(libsKernel_SRCS
        bitmap.c
        console.c
        debug.c
        hash.c
        list.c
        stdio.h)


add_library(libKernel ${libsKernel_SRCS})
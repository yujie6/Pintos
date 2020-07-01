set(vm_SRCS
        frame.c
        frame.h
        page.c
        page.h
        spt.c
        spt.h
        swap.c
        swap.h
        )

add_library(vm ${vm_SRCS})
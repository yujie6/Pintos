set(libs_SRCS
        debug.c
        limits.h
        packed.h
        round.h
        stdarg.h
        stdbool.h
        syscall-nr.h
        ustar.c
        stdio.c
        stdlib.c
        random.c
        inttypes.h
        stdint.h
        stddef.h
        string.c
        ctype.h
        )

add_library(lib ${libs_SRCS})
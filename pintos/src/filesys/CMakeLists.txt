set(filesys_SRCS
        directory.c
        directory.h
        file.c
        file.h
        filesys.c
        filesys.h
        free-map.c
        free-map.h
        inode.c
        inode.h
        fsutil.c
        fsutil.h
        off_t.h
        )

add_library(filesys ${filesys_SRCS})
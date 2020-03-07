# Pintos

## Timeline
* 2.24 basic environment done
* 2.26 add CMakeLists.txt
* 3.1 improve timer_sleep, pass 4 basic tests
* 3.5 build i386-elf-gdb, now able to use pintos-gdb to debug!
* 3.7 priority donation done, learn some mlfqs

## Useful links 
* [Official Reference Material](https://www.scs.stanford.edu/20wi-cs140/reference/)
* [Another OS project](https://github.com/cfenollosa/os-tutorial)
* [Arch Linux wiki](https://wiki.archlinux.org/index.php/Main_page)

## Environment Build Notes 
Except for the toolchain, following the official guild is enough.

My Local env: 
    
    gcc version 9.2.1 20200130 (Arch Linux 9.2.1+20200130-2)
    GNU gdb (GDB) 9.1
    
Using local gcc 
* the kernel runs smoothly
* but pintos-dbg fail

Thus we need to install i386-elf toolchain
* toolchain-build.sh -> error
* binutils good (just following the guide)
* gcc-6.2.0 -> [error](https://github.com/crosstool-ng/crosstool-ng/issues/735)
* gcc-6.4.0 -> error
* gdb-7.9.1 -> [error](https://github.com/nativeos/pkgbuild-i386-elf-toolchain/issues/1)
* gdb-8.1 / gdb-7.12 exactly the same error
* **gdb-8.2** works smoothly

Actually it's not necessary to build i386-elf-gcc, as my gcc is 
compatible with it. So I did not waste time on that any more.
(I also tried building linux kernel, that really requires a lower version
of gcc)
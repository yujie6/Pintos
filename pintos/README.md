# Pintos

## Useful links 
* [Official Reference Material](https://www.scs.stanford.edu/20wi-cs140/reference/)
* [Doc of PintOS](https://www.scs.stanford.edu/10wi-cs140/pintos/pintos_1.html)
* [Another OS project](https://github.com/cfenollosa/os-tutorial)
* [Arch Linux wiki](https://wiki.archlinux.org/index.php/Main_page)

## Environment Build Notes 
Local env: 
    
    gcc version 9.2.1 20200130 (Arch Linux 9.2.1+20200130-2)
    
Using local gcc 
* run smoothly
* pintos-dbg fail

Install i386-elf toolchain
* toolchain-build.sh -> error
* binutils good
* gcc-6.2.0 -> [error](https://github.com/crosstool-ng/crosstool-ng/issues/735)
* gcc-6.4.0 -> error
* gdb-7.9.1 -> [error](https://github.com/nativeos/pkgbuild-i386-elf-toolchain/issues/1)
* gdb-8.1 / gdb-7.12 exactly the same error
* gdb-8.2 works smoothly
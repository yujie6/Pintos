# Thread System

## Implementation Detail

## Others
#### 1. Assembly code reference
Basic format is:
```
asm [volatile] ( Assembler Template
   : Output Operands
   [ : Input Operands
   [ : Clobbers ] ])
```
And a frequently used one is memory barrier:
```c
asm volatile ("" : : : "memory")
```
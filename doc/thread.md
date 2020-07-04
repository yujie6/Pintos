# Thread System

## Implementation Detail
#### 1. Timer Sleep
Let the sleep thread go blocked. And each tick, check 
if the blocked ones are waken, if it is, make its status
ready.

#### 2. Priority change
After creating a thread, if it's priority is higher, then 
use `thread_yield`.

#### 3. Semaphore
A semaphore is a nonnegative integer together with two operators that manipulate it atomically, which are: 
* “Down” or “P”: wait for the value to become positive, then decrement it. 
* “Up” or “V”: increment the value (and wake up one waiting thread, if any).

#### 4. Lock
Compared to a semaphore, a lock has one added restriction: only the **thread that acquires a lock**, is allowed to release it. If this restriction is a problem, it’s a good sign that a semaphore should be used, instead of a lock.



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

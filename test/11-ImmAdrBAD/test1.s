.global _start

.text:

mov &_start,r0
xchg r0,25
pop 0xFFFF

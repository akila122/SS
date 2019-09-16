.global _start

.section readOnly, r

const:
.word 0xABCD

.text

_start:

mov const,0xFFFF

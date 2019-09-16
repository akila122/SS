.global _start

.data
d1:
.byte 36
d2:
.byte 37
d3:
.byte 38
d4:
.byte 39

.text
_start:

movb r0l,$d1
movb r1h,$d2

movb r0h,$d3
movb r1l,$d4

#Store by byte

movb *0xFF00,r0l
movb *0xFF00,r1h

movb *0xFF00,10

movb *0xFF00,r0h
movb *0xFF00,r1l

movb *0xFF00,10

#Change by byte

xchgb r0l,r1h

movb *0xFF00,r0l
movb *0xFF00,r1h

movb *0xFF00,10

#Change by byte

xchgb r0h,r1l

movb *0xFF00,r0h
movb *0xFF00,r1l


halt


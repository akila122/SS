.section IVT
.extern _start
.global _IVT0,_IVT1,_IVT2,_IVT3,_IVT4,_IVT5,_IVT6,_IVT7

_IVT0:

push &_start
push 0
iret

_IVT1:

movb *0xFF00,35
movb *0xFF00,69
movb *0xFF00,82  		
movb *0xFF00,82
movb *0xFF00,79
movb *0xFF00,82
movb *0xFF00,35
movb *0xFF00,10 

halt

_IVT2:

iret

_IVT3:

mov psw,0x8000
cmp *0xFF02,10
jeq end
movb *0xFF00,80
movb *0xFF00,114
movb *0xFF00,101
movb *0xFF00,115
movb *0xFF00,115
movb *0xFF00, 101
movb *0xFF00, 100
movb *0xFF00, 32
movb *0xFF00, 107
movb *0xFF00, 101
movb *0xFF00, 121
movb *0xFF00, 32
movb *0xFF00, 58
movb *0xFF00, 32
movb *0xFF00, *0xFF02
movb *0xFF00, 10 
end:
iret

_IVT4:

iret

_IVT5:

iret

_IVT6:

iret

_IVT7:

iret

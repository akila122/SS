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

movb *0xFF00, 116
movb *0xFF00, 105
movb *0xFF00, 99
movb *0xFF00, 107
movb *0xFF00, 33
movb *0xFF00, 10 

iret

_IVT3:

iret

_IVT4:

iret

_IVT5:

iret

_IVT6:

iret

_IVT7:

iret

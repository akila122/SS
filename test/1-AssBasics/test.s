
#This is the first test,
#showing how assembler resolves inputFile

				#This is fine
							#   This  	is also fine

.extern extr0,extr1,extr2

.data

data1:
.word 0xFFFF,-3020,20+3*(4-15*(0xFF|256)^0xABCD)
.byte 0xFFFF,0xAAAA,-255,0xA
#only the lower byte taken

.align 3,0xFF
#Data aligned to 3
#with fill 0xFF up to 5bytes


.equ mask,0xF0FF
.equ TnsE,extr0-extr2

data2:
.word data1
.word data2-data1
.word extr1+data2
.word extr0*(data2&mask)
.skip 20

#Address arithmetics are allowed
#but note that relocative expressions
#are not safe due to the process of linking and loading

.text

_start:

mov r0,1

add r0[data2],sp[20]

movb r1h,*0x850F

xorb r0l,r1h

mov data1, data2

mov r2,&mask

and $data1,r1

notb r5
#Lower byte taken if none defined

halt


.data

#You can continue constructing section

data3:
.skip 5


.section A, rwx

mov r0,r1
halt

.section B, rw
.align 2
.byte -20
.align 4


.bss

tst1:
.skip 8
tst2:
.skip 8

.end












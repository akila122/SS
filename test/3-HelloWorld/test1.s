.extern putChar
.global _start

.data

msg:
.byte 72,101,108,108,111,32,87,111,114,108,100,32,58,41,32,33,10
end:

.equ len,end-msg

.text

_start:

mov r0,0

do:

pushb r0[msg]

call putChar

popb r0[msg]

add r0,1

cmp r0,&len

jne do

halt


#.equ defined symbol values must be fetched with &sym!
#Adressing them without & will be decoded as memory acessing,
#for example mov r1,len would move 2bytes starting from adress 17 (wich is the value of len symbol) to r0



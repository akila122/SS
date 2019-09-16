#This test shows how Linkers algorithm can detect circular definitions

.extern f,link2
.global link1,_start

.equ a,b
.equ b,c
.equ c,d
.equ d,e
.equ e,f

.equ link1,link2

.data
.word a+10,c*3,f-4

.text

_start:

mov *0xFF00,79
mov *0xFF00,75
mov *0xFF00,10

halt


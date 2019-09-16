
.extern d1,procEx

#This test shows how TNS and RELOCS work
#Assembler will be able to resolve following declarations:

.equ a,b+2-3*4+0&(0xFFFF^0xABAB)
.equ b,c*10
.equ c,d+1
.equ d,3

#Assembler will generate TNS enteries:

.equ a1,b1+2
.equ b1,c1+3
.equ c1,d1*10

.data 
dat1:
.byte 0xA+d1 #This will generate ABS Relocation
.word d1     #This will also

.text

add r1,r2

call procEx #ABS Relocation generated

call $procEx #PC Relocation generated

call procLoc #ABS Relocation generated

call $procLoc #Assembler will resolve this, no Relocation generated

int 6

call $sFnS #PC Relocation generated

procLoc:


halt


.section text2

mov $dat2,$dat1 #First operand will be resolved but the second will generate PC Relocation

sFnS:

halt

dat2:
.byte 0xA

 





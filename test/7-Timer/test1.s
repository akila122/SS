#This test will show how user can use timer routine. The program will print "tick" in 10 seconds interval

.global _start
.text
_start:
 

mov *0xFF10,5

while:
jmp while


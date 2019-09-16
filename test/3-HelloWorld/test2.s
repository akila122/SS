.global putChar


.text


putChar:

mov *0xFF00,sp[-3]

ret

.global _start
.extern printLine,invert

.data
text:
.byte 84,104,105,115,73,115,78,111,116,65,110,97,103,114,97,109,0

.text
_start:

push &text
call $printLine
pop r0


mov *0xFF00,10

push &text
call $invert
call $printLine
pop r0


mov *0xFF00,10


halt




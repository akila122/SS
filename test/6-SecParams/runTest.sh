#!/bin/bash

assembler -o test1.o test1.s

emulator -place=.text@0xA000 test1.o -place=.data@0xB000 -place=.A@0xC000

read -r -p "TEST DONE! Press any key to continue..." key

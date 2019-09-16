#!/bin/bash

assembler -o test1.o test1.s
assembler -o test2.o test2.s
assembler -o test3.o test3.s
emulator test1.o test2.o test3.o
read -r -p "TEST DONE! Press any key to continue..." key

#!/bin/bash

assembler -o test1.o test1.s
emulator test1.o
read -r -p "TEST DONE! Press any key to continue..." key

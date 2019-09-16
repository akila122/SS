#!/bin/bash
sudo cp IVTSetup.s /usr/bin
assembler test1.s -o test1.o
emulator test1.o

read -r -p "TEST DONE! Press any key to continue..." key

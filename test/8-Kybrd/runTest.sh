#!/bin/bash
sudo cp IVTSetup.s /usr/bin
assembler test1.s -o test1.o
emulator test1.o

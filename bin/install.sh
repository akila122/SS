#!/bin/bash
if sudo cp assembler /usr/bin; then
	echo Command assembler enabled
	if sudo cp emulator /usr/bin; then
		echo Command emulator enabled
		if sudo cp IVTSetup.s /usr/bin; then
			echo IVTSetup.s copied
			echo Installation completed
		else
			echo Failed to move IVTSetup. s to bin
		fi
	else
		echo Failed to move emulator to bin
	fi
else
    echo Failed to move assembler to bin
fi

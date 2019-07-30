#!/bin/sh
diet gcc -static -g -o loader loader.c -Wl,--section-start -Wl,.text=0x100800 -Wl,--section-start -Wl,.note.gnu.build-id=0x200000


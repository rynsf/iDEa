#!/bin/sh
#gcc iDEa.c -lSDL2 -Wall -o iDEa && ./iDEa

gcc iDEa.c -g -O0 -lSDL2 -Wall -o iDEa && valgrind ./iDEa

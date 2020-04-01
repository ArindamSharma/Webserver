#!/bin/bash
clear
gcc stage2.c -lpthread
./a.out
rm a.out
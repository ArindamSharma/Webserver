#!/bin/bash
clear
gcc stage5.c -lpthread
./a.out
rm a.out
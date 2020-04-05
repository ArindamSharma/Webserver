#!/bin/bash
clear
gcc stage3.c -lpthread
./a.out
rm a.out
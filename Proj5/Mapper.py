#!/usr/bin/python3
# -*- coding: utf-8 -*-
from sys import stdin, stdout
while True:

    line = stdin.readline().strip()

    if line == "":
        break;
    words = line.split()

    for i in range(len(words)-1):
        stdout.write(words[i] + " " + words[i+1] +  "\t1\n")

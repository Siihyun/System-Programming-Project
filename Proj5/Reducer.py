#!/usr/bin/python3
# -*- coding: utf-8 -*-
from sys import stdin, stdout

lib = {}
lib1 =[]

while True:
    
    line = stdin.readline().strip()
    if line == "":
        break
    word, cnt = line.split('\t')
    if word in lib:
        lib[word] += int(cnt)
    else:
        lib[word] = int(cnt)
        lib1.append(word)

lib1.sort()

for word in lib1:
    stdout.write(word + "\t" + str(lib[word]) + "\n")

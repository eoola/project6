#! /usr/bin/env python3

import random
import string
import sys

if (len(sys.argv) == 1):
    loop = 1000000
    repeat = 20
else:
    loop = int(sys.argv[1])
    repeat = int(sys.argv[2])

for i in range(loop):
    x = ''
    letter = random.choice(string.ascii_lowercase + '\n')
    if letter == '\n':
        x += letter
    else:
        for i in range(int(random.random() * repeat) + 1):
            x += letter
    print(x, end='')


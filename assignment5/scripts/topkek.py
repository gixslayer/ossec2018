import sys
from itertools import groupby


def build_bit_string():
    bit_string = ''

    for line in sys.stdin.readlines():
        for char in line:
            bit_string += bin(ord(char))[2:].zfill(8)

    return bit_string


def encode_bit_string(bit_string):
    topkeks = []
    
    for b, g in groupby(bit_string):
        prefix = 'TOP' if b == '1' else 'KEK'
        suffix = '!' * len(list(g))
        topkeks.append(prefix + suffix)
    
    print(' '.join(topkeks))


encode_bit_string(build_bit_string())
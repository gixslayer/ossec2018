import sys


def process_word(word):
    bit = '1' if word[0:3] == 'TOP' else '0'
    return bit * (len(word) - 3)


def build_bit_string():
    bit_string = ''

    for line in sys.stdin.readlines():
        for word in line.split(' '):
            bit_string += process_word(word)

    return bit_string


def decode_bit_string(bit_string):
    for i in range(0, len(bit_string) // 8):
        char_bits = bit_string[i*8:(i+1)*8]
        char = chr(int(char_bits, 2))
        sys.stdout.write(char)
    sys.stdout.flush()


decode_bit_string(build_bit_string())
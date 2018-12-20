import sys

def print_context(words, index, length):
    context = []

    for i in range(-length, length):
        context.append(words[index + i])
    
    print(' '.join(context))


def read_word_list():
    with open('wordlist.txt', 'r') as wl:
        return [line.strip('\n') for line in wl.readlines()]


with open('flag2', 'r') as f:
    line = f.readline()
    words = line.split(' ')
    wordlist = read_word_list()
    for i in range(0, len(words)):
        word = words[i].lower()
        if word in ['remember', 'another', 'language', 'until', 'positive', 'sugar']:
            print_context(words, i, 10)
        #if len(word) > 4 and word in wordlist:
            #print(word)
            

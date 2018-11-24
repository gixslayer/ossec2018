all: read write

read: read-proc.c
	gcc -o read-proc -Wall -Wextra read-proc.c

write: write-proc.c
	gcc -o write-proc -Wall -Wextra write-proc.c

clean:
	rm -f read-proc write-proc

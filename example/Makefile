CC=gcc
CFLAGS=-c -Wall -Wextra -O3
LDFLAGS=btree.a

output: main.o 
	$(CC) main.o -o example $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm *.o && rm example

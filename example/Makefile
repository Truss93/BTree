CC=gcc
CFLAGS=-c -Wall -Wextra
LDFLAGS=btree.a

output: main.o 
	$(CC) main.o -o example $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm *.o && rm example
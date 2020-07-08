CC=gcc
CFLAGS=-c -Wall -Wextra -O3

output: btree.o
	ar rcs btree.a btree.o

btree.o: btree.c
	$(CC) $(CFLAGS) btree.c -o btree.o

clean:
	rm *.o && rm *.a

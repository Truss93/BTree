CC=gcc
CFLAGS=-c -Wall -Wextra
LDFLAGS = -lncurses

output: main.o vbtree.o btvis_nc.o
	$(CC) main.o vbtree.o btvis_nc.o -o visual $(LDFLAGS)



%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@


clean:
	rm *.o && rm visual
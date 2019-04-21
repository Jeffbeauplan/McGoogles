CC=gcc
CFLAGS=-I. -pthread -std=c99
DEPS = McGoogles.h
OBJ = McGoogles.o main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
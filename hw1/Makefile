.PHONY: clean

CC = gcc
CFLAGS = -Wall -g -D_GNU_SOURCE -fPIC
LDFLAGS = -shared


sandbox.so: sandbox.o
	$(CC) -o $@ $^ $(LDFLAGS)

sandbox.o: sandbox.c
	$(CC) -c $< $(CFLAGS)

clean:
	- rm -f sandbox.so sandbox.o
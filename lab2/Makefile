CC = gcc
CFLAGS=-Wall -Wextra
TARGET=SimpleCache

all:
	$(CC) $(CFLAGS) -g SimpleProgram.c SimpleCache.c -o $(TARGET)

break:
	$(CC) $(CFLAGS) BreakProgram.c SimpleCache.c -o BreakProgram

break2:
	$(CC) $(CFLAGS) Break2.c SimpleCache.c -o Break2

l1:
	$(CC) $(CFLAGS) L1Program.c L1Cache.c -o L1Cache

clean:
	rm $(TARGET)

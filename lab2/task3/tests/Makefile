CC = gcc
CFLAGS=-Wall -Wextra

SRCS = $(wildcard *.c)
TESTS = $(patsubst %.c,%,$(SRCS))

all: $(TESTS)

test: $(TESTS)
	@for test in $^; do \
		echo "Running test $$test"; \
		./$$test; \
		echo; \
	done

%: %.c
	$(CC) $(CFLAGS) -g $< ../L2Cache.c -o $@

clean:
	rm -f $(TESTS)

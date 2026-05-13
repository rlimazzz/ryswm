CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -O2
LDLIBS ?= -lX11

all: ryswm

ryswm: main.c
	$(CC) $(CFLAGS) main.c -o $@ $(LDLIBS)

clean:
	rm -f ryswm

.PHONY: all clean
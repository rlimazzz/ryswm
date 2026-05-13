CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -O2 -Iinclude
LDLIBS ?= -lX11

SRCDIR := src
OBJDIR := build
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

all: ryswm

ryswm: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf ryswm $(OBJDIR)

.PHONY: all clean
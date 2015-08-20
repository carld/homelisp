# vim: tabstop=8 noexpandtab

CC      = gcc
CFLAGS += -Wall
CFLAGS += -pedantic
CFLAGS += -ansi
CFLAGS += -g
CFLAGS += -ggdb
CFLAGS += -D_BSD_SOURCE -D_DEFAULT_SOURCE
CFLAGS += -pg
CFLAGS += -fprofile-arcs -ftest-coverage #-fomit-frame-pointer

LDFLAGS += -pg
LDFLAGS += -fprofile-arcs -ftest-coverage
LDFLAGS += -lgc

all: repl 

repl: driver.c debug.c expr.c read.c eval_r.c print.c prims.c
	$(CC) -o $@ $(CFLAGS) driver.c $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o *.gcda *.gcno repl

.PHONY: clean all test


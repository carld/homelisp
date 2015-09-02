# vim: tabstop=8 noexpandtab

CC      = gcc
CFLAGS += -Wall
CFLAGS += -pedantic
CFLAGS += -ansi
CFLAGS += -g
CFLAGS += -ggdb
CFLAGS += -D_BSD_SOURCE -D_DEFAULT_SOURCE
#CFLAGS += -pg
CFLAGS += -fomit-frame-pointer
#CFLAGS += -DDEBUG_TOKEN=1
#CFLAGS += -DDEBUG=1
#CFLAGS += -fprofile-arcs -ftest-coverage

#LDFLAGS += -pg
#LDFLAGS += -fprofile-arcs -ftest-coverage
LDFLAGS += -lgc

all: repl
	./lisp-tests.sh

repl: repl.c debug.c expr.c read.c eval_r.c print.c prims.c load.c
	$(CC) -o $@   $(CFLAGS)   repl.c   $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -fv *.o *.gcda *.gcno repl

.PHONY: clean all test


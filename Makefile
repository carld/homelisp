# vim: tabstop=8 noexpandtab

CC      = gcc
CFLAGS += -Wall
CFLAGS += -pedantic
CFLAGS += -ansi
CFLAGS += -g
CFLAGS += -ggdb
CFLAGS += -D_BSD_SOURCE -D_DEFAULT_SOURCE
#CFLAGS += -pg
CFLAGS += -O2
#CFLAGS += -fomit-frame-pointer
#CFLAGS += -DDEBUG_TOKEN=1
#CFLAGS += -DDEBUG=1
#CFLAGS += -fprofile-arcs -ftest-coverage

#LDFLAGS += -pg
#LDFLAGS += -fprofile-arcs -ftest-coverage
LDFLAGS += -lgc

all: repl lexer.png parser.png
	./lisp-tests.sh

repl: repl.c debug.c expr.c read.c eval_r.c print.c prims.c load.c
	wc -l *.c
	$(CC) -o $@   $(CFLAGS)   repl.c   $(LDFLAGS)
	size $@

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -fv *.o *.gcda *.gcno repl

lexer.png: lexer-state.dot
	dot -Tpng -o $@ $^

parser.png: parser-state.dot
	dot -Tpng -o $@ $^

.PHONY: clean all test


UNAME_S=$(shell uname -s)
EMACS_ROOT ?= ../..

EMACS ?= emacs
CC      = gcc
LD      = gcc
CFLAGS = -I$(EMACS_ROOT)/src
CFLAGS = -std=gnu99 -ggdb3 -Wall -fPIC $(CPPFLAGS)

.PHONY : format clean test

all: zstd-core.so

zstd-core.so: zstd-core.o
	$(LD) -shared -o $@ $^ -lzstd

zstd_core.o: zstd-core.c
	$(CC) $(CFLAGS) -c -o $@ $<

test:
	$(EMACS) -Q -batch -L . -l test/zstd-test.el -f ert-run-tests-batch-and-exit

clean:
	-rm -f zstd-core.so zstd-core.o

format:
	clang-format -i *.c

.PHONY : test

EMACS_ROOT ?= ../..
CC      = gcc
LD      = gcc
CPPFLAGS = -I$(EMACS_ROOT)/src
CFLAGS = -std=gnu99 -ggdb3 -Wall -fPIC $(CPPFLAGS)

UNAME_S=$(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	EMACS ?= /Applications/Emacs.app/Contents/MacOS/Emacs
else
	EMACS ?= emacs
endif

LOADPATH = -L .

all: zstd-core.so

zstd-core.so: zstd-core.o
	$(LD) -shared $(LDFLAGS) -o $@ $^ -lzstd

zstd-core.o: zstd-core.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f zstd-core.so zstd-core.o

test:
	$(EMACS) -Q -batch $(LOADPATH) \
		-l test/test.el \
		-f ert-run-tests-batch-and-exit

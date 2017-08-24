SHELL=/bin/sh
CC=gcc
CFLAGS=-I. -O2 -Wall -pedantic -std=gnu99
LIBS=-lX11
OUT=kbledctl
PREFIX=/usr
INSTALL_DEST=bin/kbledctl
SRCS=linux-keys.c main.c

all:
	@echo " -- Getting needed sources..."
	./dep.sh || exit 1
	@echo " -- Success, compiling..."
	$(CC) $(CFLAGS) $(LIBS) -o $(OUT) $(SRCS) || exit 2
	@echo " ++ Output: $(OUT)"
	@echo "done"

install:
	@echo " -- Installing $(OUT) into $(PREFIX)/$(INSTALL_DEST)"
	cp $(OUT) $(PREFIX)/$(INSTALL_DEST) || exit 1
	@echo " ++ Done"

.PHONY:all,install

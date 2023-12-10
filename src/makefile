SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .c .o

CC=gcc
WARNINGS=-Wall -Wextra

CFLAGS=-O3

ifdef DEBUG
	CFLAGS=-g -D DEBUG
endif

CFLAGS += $(WARNING)
LDLIBS=-lgd

BUILDDIR = build
SRC := $(shell find . -name '*.c')
OBJ := $(addprefix $(BUILDDIR)/,$(SRC:%.c=%.o))
DEPS := $(shell find . -name '*.h')
OUT_NAME = old-photo-paralelo-A

$(BUILDDIR)/%.o : %.c $(DEPS) | $(BUILDDIR)
	$(CC) -c $< $(CFLAGS) $(LDLIBS) -o $@

$(OUT_NAME) : $(OBJ)
	$(CC) $^ $(CFLAGS) $(LDLIBS) -o $@

.PHONY : $(BUILDDIR)
$(BUILDDIR):
	mkdir -p $@

.PHONY : clean
clean:
	rm $(BUILDDIR)/*.o
	rm $(OUT_NAME)


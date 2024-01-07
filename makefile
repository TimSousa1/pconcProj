CFLAGS=-Wall -Wextra

ifdef THREAD_OFF
	CFLAGS += -D THREAD_OFF
endif

all:
	gcc a.c image-lib.c -lgd $(CFLAGS) -ggdb

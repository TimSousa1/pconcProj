CC=gcc

WARNINGS=-Wall -Wextra

LDLIBS=-lgd
CFLAGS=-O3

OBJ=main.o read.o image.o image-lib.o clock.o
OUT_NAME=old-photo-paralelo-A

SRC=../src
INC=../include
DEPS=$(INC)/main.h $(INC)/image-lib.h

CFLAGS += -I$(INC)
CFLAGS += $(WARNINGS)

ifdef DEBUG
	CFLAGS=-g -D DEBUG
endif


%.o : $(SRC)/%.c $(DEPS)
	$(CC) -c $< $(CFLAGS) $(LDLIBS) -g

$(OUT_NAME) : $(OBJ) $(DEPS)
	$(CC) $(OBJ) -o $@ $(CFLAGS) $(LDLIBS) -g

clean:
	rm *.o
	rm $(OUT_NAME)

LINKERS=-lgd
SRC=../src
WARNINGS=-Wall -Wextra

CFLAGS=-O3
OUT_NAME=old-photo-paralelo-A

ifdef DEBUG
	CFLAGS=-g -D DEBUG
endif

compile::
	gcc -c $(SRC)/main.c $(LINKERS) $(CFLAGS) $(WARNINGS)
	gcc -c $(SRC)/read.c $(LINKERS) $(CFLAGS) $(WARNINGS)
	gcc -c $(SRC)/image.c $(LINKERS) $(CFLAGS) $(WARNINGS)
	gcc -c $(SRC)/image-lib.c $(LINKERS) $(CFLAGS) $(WARNINGS)
	gcc main.o read.o image.o image-lib.o $(LINKERS) $(CFLAGS) $(WARNINGS) -o $(OUT_NAME)

clean::
	rm *.o
	rm $(OUT_NAME)

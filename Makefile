CC = gcc
CFLAGS = -g -Wall `pkg-config --cflags readline`
LDFLAGS = `pkg-config --libs readline`
TARGET = chess
PREFIX = /usr/local

objects = main.o fen.o io.o moves.o check.o

${TARGET}: ${objects}
	${CC} ${CFLAGS} -o ${TARGET} ${objects} ${LDFLAGS}

main.o io.o fen.o moves.o check.o: game.h

.PHONY: clean install 
clean:
	rm -f ${objects} ${TARGET}
install:
	cp ${TARGET} ${PREFIX}/bin/

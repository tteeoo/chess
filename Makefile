CC = gcc
CFLAGS = -Wall
TARGET = chess
PREFIX = /usr/local

objects = main.o fen.o board.o

${TARGET}: ${objects}
	${CC} ${CFLAGS} -o ${TARGET} ${objects}

main.o fen.o board.o: board.h

.PHONY: clean install 
clean:
	rm -f ${objects} ${TARGET}

install:
	cp ${TARGET} ${PREFIX}/bin/

#include <stdlib.h>
#include <ctype.h>

#include "board.h"

char ptoc(piece p) {
	if (p == 0)
		return '-';
	char c;
	switch (p & ~(white | black)) {
		case pawn:
			c = 'p';
			break;
		case knight: 
			c = 'n';
			break;
		case bishop: 
			c = 'b';
			break;
		case rook: 
			c = 'r';
			break;
		case queen: 
			c = 'q';
			break;
		case king: 
			c = 'k';
			break;
	}
	if ((p & white) == white)
		c = toupper(c);
	return c;
}

piece ctop(char c) {
	piece p = black;
	if (isupper(c)) {
		p = white;
		c = tolower(c);
	}
	switch (c) {
		case 'p':
			p |= pawn;
			break;
		case 'n':
			p |= knight;
			break;
		case 'b':
			p |= bishop;
			break;
		case 'r':
			p |= rook;
			break;
		case 'q':
			p |= queen;
			break;
		case 'k':
			p |= king;
			break;
	}
	return p;
}

board* parse_fen(char* fen) {
	board* b = (board*)malloc(sizeof(board));

	// TODO

	return b;
}

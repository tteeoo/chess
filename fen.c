#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
	piece p = isupper(c) ? white : black;

	switch (tolower(c)) {
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

void load_fen(char* fen, board b) {
	int file = 0;
	int rank = 0;

	for (int i = 0; i < strlen(fen); i++) {
		if (fen[i] == ' ') {
			// TODO: handle other FEN data
			break;
		} else if (fen[i] == '/') {
			file = 0;
			rank++;
		} else {
			if (isdigit(fen[i])) {
				file += (int)(fen[i] - '0');
			} else {
				b[rank * 8 + file] = ctop(fen[i]);
				file++;
			}
		}
	}
}

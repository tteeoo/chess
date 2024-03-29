// Chess implemented in C; fen.c implements parsing of the FEN notation.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "game.h"

// Piece to char
char ptoc(int piece) {
	if (piece == 0)
		return '-';

	char c;
	switch (PIECE_TYPE(piece)) {
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
	if (PIECE_COLOR(piece) == white)
		c = toupper(c);

	return c;
}

// Char to piece
int ctop(char c) {
	int piece = isupper(c) ? white : black;

	switch (tolower(c)) {
		case 'p':
			piece |= pawn;
			break;
		case 'n':
			piece |= knight;
			break;
		case 'b':
			piece |= bishop;
			break;
		case 'r':
			piece |= rook;
			break;
		case 'q':
			piece |= queen;
			break;
		case 'k':
			piece |= king;
			break;
		default:
			piece = 0;
	}

	return piece;
}

// Populates game board from FEN string
void load_fen(char* fen, game* g) {
	int file = 0;
	int rank = 7;

	// Parse FEN data into board
	for (int i = 0; i < strlen(fen); i++) {
		if (fen[i] == ' ') {
			// TODO: handle other FEN data
			break;
		} else if (fen[i] == '/') {
			file = 0;
			rank--;
		} else {
			if (isdigit(fen[i])) {
				file += (int)(fen[i] - '0');
			} else {
				g->board[rank * 8 + file] = ctop(fen[i]);
				file++;
			}
		}
	}

	// Create tile lists from board
	piece_list* pl_white = NULL;
	piece_list* pl_black = NULL;
	for (int i = 0; i < 64; i++) {
		if (g->board[i] != 0) {
			// King tile
			if (PIECE_TYPE(g->board[i]) == king) {
				if (PIECE_COLOR(g->board[i]) == white)
					g->king_tiles[0] = i;
				else
					g->king_tiles[1] = i;
			}

			piece_list* p = malloc(sizeof(piece_list*));
			p->tile = i;
			switch (PIECE_COLOR(g->board[i])) {
				case white:
					APPEND_LIST(pl_white, g->pieces[0], p);
					break;
				case black:
					APPEND_LIST(pl_black, g->pieces[1], p);
					break;
			}
		}
	}
}

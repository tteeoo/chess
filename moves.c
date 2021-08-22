// Chess implemented in C; moves.c implements the generation of valid moves.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdlib.h>

#include "game.h"

static const int directions[] = { 8, -8, -1, 1, 7, -7, 9, -9 };

int* tiles_from_edge[64];

void compute_move_data() {
	for (int file = 0; file < 8; file++) {
		for (int rank = 0; rank < 8; rank++) {
			int i = rank * 8 + file;
			int north = 7 - rank;
			int south = rank;
			int west = file;
			int east = 7 - file;
			tiles_from_edge[i] = malloc(sizeof(int) * 8);
			tiles_from_edge[i][0] = north;
			tiles_from_edge[i][1] = south;
			tiles_from_edge[i][2] = west;
			tiles_from_edge[i][3] = east;
			tiles_from_edge[i][4] = MIN(north, west);
			tiles_from_edge[i][5] = MIN(south, east);
			tiles_from_edge[i][6] = MIN(north, east);
			tiles_from_edge[i][7] = MIN(south, west);
		}
	}
}

move* get_sliding_moves(int board[64], int tile) {
	move* m = NULL;
	move* head = NULL;
	int piece = board[tile];
	int di_start = (PIECE_TYPE(piece) == bishop) ? 4 : 0;
	int di_end = (PIECE_TYPE(piece) == rook) ? 4 : 8;

	for (int di = di_start; di < di_end; di++) {
		for (int i = 0; i < tiles_from_edge[tile][di]; i++) {
			int destination = tile + directions[di] * (i + 1);
			int occupying = board[destination];

			if (HAS_MASK(occupying, PIECE_COLOR(piece)))
				break;

			move* np = malloc(sizeof(move*));
			np->start = tile;
			np->end = destination;
			np->next = NULL;
			if (!m) {
				m = np;
				head = np;
			} else {
				m->next = np;
				m = np;
			}

			if (HAS_MASK(occupying, PIECE_OCOLOR(piece)))
				break;
		}
	}

	return head;
}

move* get_piece_moves(int board[64], int tile) {
	if (SLIDING_PIECE(board[tile])) {
		return get_sliding_moves(board, tile);
	}

	// TODO: moves for other pieces
	
	return NULL;
}

move* get_moves(game* g) {
	move* m = NULL;
	for (int i = 0; i < 64; i++) {
		int piece = g->board[i];
		if (HAS_MASK(piece, g->turn)) {
			move* mp = get_piece_moves(g->board, i);
			if (!mp) {
				if (!m)
					m = mp;
				else {
					m->next = mp;
					m = m->next;
				}
			}
		}
	}

	return m;
}

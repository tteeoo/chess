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
			int north = rank;
			int south = 7 - rank;
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
	int piece = board[tile];
	int di_start = HAS_MASK(piece, bishop) ? 4 : 0;
	int di_end = HAS_MASK(piece, rook) ? 4 : 8;

	for (int di = di_start; di < di_end; di++) {
		for (int i = 0; i < tiles_from_edge[tile][di]; i++) {
			int destination = tile + directions[di] * (i + 1);
			int occupying = board[destination];

			if (HAS_MASK(occupying, PIECE_COLOR(piece)))
				break;

			move n = {.start = tile, .end = destination, .next = NULL};
			if (!m)
				m = &n;
			else
				m->next = &n;

			if (HAS_MASK(occupying, PIECE_OCOLOR(piece)))
				break;
		}
	}

	return m;
}

move* get_moves(game* g) {
	move* m = malloc(sizeof(move));
	for (int i = 0; i < 64; i++) {
		int piece = g->board[i];
		if (HAS_MASK(piece, g->turn)) {
			if (SLIDING_PIECE(piece)) {
				get_sliding_moves(g->board, i);
			}

			// TODO: moves for other pieces
		}
	}

	return m;
}

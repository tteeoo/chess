#include <stdlib.h>

#include "board.h"

static const int directions[] = { 8, -8, -1, 1, 7, -7, 9, -9 };

struct {
	int north;
	int south;
	int west;
	int east;
	int northwest;
	int northeast;
	int southwest;
	int southeast;
} typedef tile_data;

static tile_data tiles_from_edge[64];

void compute_move_data() {
	for (int file = 0; file < 8; file++) {
		for (int rank = 0; rank < 8; rank++) {
			int i = rank * 8 + file;
			int north = rank;
			int south = 7 - rank;
			int west = file;
			int east = 7 - file;
			tiles_from_edge[i].north = north;
			tiles_from_edge[i].south = south;
			tiles_from_edge[i].west = west;
			tiles_from_edge[i].east = east;
			tiles_from_edge[i].northwest = MIN(north, west);
			tiles_from_edge[i].northeast = MIN(north, east);
			tiles_from_edge[i].southwest = MIN(south, west);
			tiles_from_edge[i].southeast = MIN(south, east);
		}
	}
}

move* get_sliding_moves(int piece, int tile) {
	return NULL;
}

move* get_moves(game g) {
	move* m = malloc(sizeof(move));
	for (int i = 0; i < 64; i++) {
		int piece = g.board[i];
		if (HAS_MASK(piece, g.turn)) {
			if (SLIDING_PIECE(piece)) {
				get_sliding_moves(piece, i);
			}
		}
	}

	return m;
}

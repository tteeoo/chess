// Chess implemented in C; moves.c implements the generation of valid moves.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdlib.h>

#include "game.h"

static const int directions[] = { 8, -8, -1, 1, 7, -7, 9, -9 };
static const int knight_jump_offsets[] = { 15, 17, -17, -15, 10, -6, 6, -10 };

int tiles_from_edge[64][8];
int knight_jumps[64][8];

int max(int a, int b) {
	int diff = a - b;
	int dsgn = diff >> 31;
	return a - (diff & dsgn);
}

int min(int a, int b) {
	int diff = a - b;
	int dsgn = diff >> 31;
	return b + (diff & dsgn);
}

int abs(int a) {
	int s = a >> 31;
	a ^= s;
	a -= s;
	return a;
}

void compute_move_data() {
	for (int file = 0; file < 8; file++) {
		for (int rank = 0; rank < 8; rank++) {
			int tile = rank * 8 + file;
			int y = tile / 8;
			int x = tile % 8;

			int north = 7 - rank;
			int south = rank;
			int west = file;
			int east = 7 - file;
			tiles_from_edge[tile][0] = north;
			tiles_from_edge[tile][1] = south;
			tiles_from_edge[tile][2] = west;
			tiles_from_edge[tile][3] = east;
			tiles_from_edge[tile][4] = min(north, west);
			tiles_from_edge[tile][5] = min(south, east);
			tiles_from_edge[tile][6] = min(north, east);
			tiles_from_edge[tile][7] = min(south, west);

			for (int i = 0; i < 8; i++) {
				int jump_tile = tile + knight_jump_offsets[i];
				if (jump_tile > -1 && jump_tile < 64) {
					int jumpy = jump_tile / 8;
					int jumpx = jump_tile % 8;
					int move_dst = max(abs(x - jumpx), abs(y - jumpy));
					if (move_dst == 2)
						knight_jumps[tile][i] = jump_tile;
					else
						knight_jumps[tile][i] = -1;
				}
			}
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

move* get_knight_moves(int board[64], int tile) {
	move* m = NULL;
	move* head = NULL;
	int piece = board[tile];

	for (int i = 0; i < 8; i++) {
		if (knight_jumps[tile][i] == -1)
			continue;
		if (HAS_MASK(board[knight_jumps[tile][i]], PIECE_COLOR(piece)))
			continue;

		move* np = malloc(sizeof(move*));
		np->start = tile;
		np->end = knight_jumps[tile][i];
		np->next = NULL;
		if (!m) {
			m = np;
			head = np;
		} else {
			m->next = np;
			m = np;
		}
	}

	return head;
}

move* get_piece_moves(int board[64], int tile) {
	if (SLIDING_PIECE(board[tile])) {
		return get_sliding_moves(board, tile);
	}
	if (PIECE_TYPE(board[tile]) == knight) {
		return get_knight_moves(board, tile);
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

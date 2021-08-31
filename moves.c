// Chess implemented in C; moves.c implements move validation.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdlib.h>
#include <stdio.h>

#include "game.h"

#define COL_I(piece) ((HAS_MASK(piece, white)) ? 0 : 1)

// Appends a move to a linked list
#define APPEND_MOVE(m, head, nm) { \
		if (!m) { \
			m = nm; \
			head = nm; \
		} else { \
			m->next = nm; \
			m = nm; \
		} \
	}

// Values used to calculate valid move information
static const int directions[] = { 8, -8, -1, 1, 7, -7, 9, -9 };
static const int knight_jump_offsets[] = { 15, 17, -17, -15, 10, -6, 6, -10 };
static const int pawn_capture_offsets[2][2] = { { 7, 9 }, { -9, -7 } };
static const int pawn_capture_directions[2][2] = { { 4, 6 }, { 7, 5 } };
static const int pawn_locations[2][4] = { { 1, 6, 8, 3 }, { 6, 1, -8, 4 } };

// Stores information about vailid moves
int tiles_from_edge[64][8];
int knight_jumps[64][8];
int pawn_captures_white[64][2];
int pawn_captures_black[64][2];

// Branchless maximum function
static int max(int a, int b) {
	int diff = a - b;
	int dsgn = diff >> 31;
	return a - (diff & dsgn);
}

// Branchless minimum function
static int min(int a, int b) {
	int diff = a - b;
	int dsgn = diff >> 31;
	return b + (diff & dsgn);
}

static int two_pawn_push(move* m, int board[64]) {
	if (m) {
		int piece = board[m->end];
		int c = COL_I(piece);
		if ((m->start / 8 == pawn_locations[c][0]) &&
			(m->end / 8 == pawn_locations[c][3])) {
			return 1;
		}
	}
	return 0;
}

// Computes information about valid moves
void compute_move_data() {

	for (int tile = 0; tile < 64; tile++) {

		// Calculate information for sliding moves
		int rank = tile / 8;
		int file = tile % 8;
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

		// Calculate knight moves
		for (int i = 0; i < 8; i++) {
			int jump_tile = tile + knight_jump_offsets[i];
			if (jump_tile > -1 && jump_tile < 64) {
				int jump_rank = jump_tile / 8;
				int jump_file = jump_tile % 8;
				int move_dst = max(abs(file - jump_file), abs(rank - jump_rank));
				if (move_dst == 2)
					knight_jumps[tile][i] = jump_tile;
				else
					knight_jumps[tile][i] = -1;
			}
		}

		// Calculate pawn attacks
		pawn_captures_white[tile][0] = -1;
		pawn_captures_black[tile][1] = -1;
		if (file > 0) {
			if (rank < 7)
				pawn_captures_white[tile][0] = tile + pawn_capture_offsets[0][0];
			if (rank > 0)
				pawn_captures_black[tile][0] = tile + pawn_capture_offsets[1][0];
		}
		if (file < 7) {
			if (rank < 7)
				pawn_captures_white[tile][0] = tile + pawn_capture_offsets[0][1];
			if (rank > 0)
				pawn_captures_black[tile][0] = tile + pawn_capture_offsets[1][1];
		}
	}
}

// Gets moves for a pawn
static move* get_pawn_moves(game* g, int tile) {
	move* m = NULL;
	move* head = NULL;
	int* board = g->board;
	int piece = board[tile];
	int c = COL_I(piece);
	int forward = pawn_locations[c][2];
	int rank = tile / 8;
	int next_promotion = rank == pawn_locations[c][1];

	// Forward
	int forward_tile = tile + forward;
	if (board[forward_tile] == 0) {
		move* nm = malloc(sizeof(move*));
		nm->start = tile;
		nm->end = forward_tile;
		nm->en_passant = 0;
		if (next_promotion)
			nm->promotion = 1;
		else
			nm->promotion = 0;
		nm->next = NULL;
		APPEND_MOVE(m, head, nm);
		if (rank == pawn_locations[c][0]) {
			int two_forward = forward_tile + forward;
			if (board[two_forward] == 0) {
				move* nm = malloc(sizeof(move*));
				nm->start = tile;
				nm->end = two_forward;
				nm->en_passant = 0;
				nm->promotion = 0;
				nm->next = NULL;
				APPEND_MOVE(m, head, nm);
			}
		}
	}

	// Captures
	for (int i = 0; i < 2; i++) {
		if (tiles_from_edge[tile][pawn_capture_directions[c][i]] > 0) {
			int capture_direction = directions[pawn_capture_directions[c][i]];
			int destination = tile + capture_direction;
			int occupying = board[destination];
			if (HAS_MASK(occupying, PIECE_OCOLOR(piece))) {
				move* nm = malloc(sizeof(move*));
				nm->start = tile;
				nm->end = destination;
				nm->en_passant = 0;
				if (next_promotion)
					nm->promotion = 1;
				else
					nm->promotion = 0;
				nm->next = NULL;
				APPEND_MOVE(m, head, nm);

			// En passant
			} else if ((tile / 8) == pawn_locations[!c][3]) {
				move* last_move = g->moves_tail;
				if (two_pawn_push(last_move, board)) {
					if (last_move->end == destination + pawn_locations[!c][2]) {
						move* nm = malloc(sizeof(move*));
						nm->start = tile;
						nm->end = destination;
						nm->en_passant = 1;
						nm->promotion = 0;
						nm->next = NULL;
						APPEND_MOVE(m, head, nm);
					}
				}
			}
		}
	}

	return head;
}

// Gets moves for a sliding piece
static move* get_sliding_moves(int board[64], int tile) {
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

			move* nm = malloc(sizeof(move*));
			nm->start = tile;
			nm->end = destination;
			nm->en_passant = 0;
			nm->promotion = 0;
			nm->next = NULL;
			APPEND_MOVE(m, head, nm);

			if (HAS_MASK(occupying, PIECE_OCOLOR(piece)))
				break;
		}
	}

	return head;
}

// Gets moves for a knight
static move* get_knight_moves(int board[64], int tile) {
	move* m = NULL;
	move* head = NULL;
	int piece = board[tile];

	for (int i = 0; i < 8; i++) {
		if (knight_jumps[tile][i] == -1)
			continue;
		if (HAS_MASK(board[knight_jumps[tile][i]], PIECE_COLOR(piece)))
			continue;

		move* nm = malloc(sizeof(move*));
		nm->start = tile;
		nm->end = knight_jumps[tile][i];
		nm->en_passant = 0;
		nm->promotion = 0;
		nm->next = NULL;
		APPEND_MOVE(m, head, nm);
	}

	return head;
}

// Gets moves for a specific piece
move* get_piece_moves(game* g, int tile) {
	if (SLIDING_PIECE(g->board[tile])) {
		return get_sliding_moves(g->board, tile);
	}
	if (PIECE_TYPE(g->board[tile]) == knight) {
		return get_knight_moves(g->board, tile);
	}
	if (PIECE_TYPE(g->board[tile]) == pawn) {
		return get_pawn_moves(g, tile);
	}

	// TODO: moves for king
	
	return NULL;
}

// Gets moves for the current side
move* get_moves(game* g) {
	move* m = NULL;
	move* head = NULL;
	for (int i = 0; i < 64; i++) {
		int piece = g->board[i];
		if (HAS_MASK(piece, g->turn)) {
			move* nm = get_piece_moves(g, i);
			APPEND_MOVE(m, head, nm);
			for (; m; m = m->next);
		}
	}

	return head;
}

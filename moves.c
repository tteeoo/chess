// Chess implemented in C; moves.c implements move validation.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdlib.h>
#include <stdio.h>

#include "game.h"

// Values used to calculate valid move information
static const int directions[] = { 8, -8, -1, 1, 7, -7, 9, -9 };
static const int knight_jump_offsets[] = { 15, 17, -17, -15, 10, -6, 6, -10 };
static const int pawn_capture_directions[2][2] = { { 4, 6 }, { 7, 5 } };
// Start rank, next promotion rank, forward offset, two tile push rank
static const int pawn_locations[2][4] = { { 1, 6, 8, 3 }, { 6, 1, -8, 4 } };

// Stores information about valid moves
int tiles_from_edge[64][8];
int knight_jumps[64][8];
int king_moves[64][8];
int king_distances[64][64];

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

// Returns whether a move was a double pawn push
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

// Delete an item from a piece_list based on its tile value
static piece_list* del_piece(piece_list* head, int tile) {
	piece_list* p = head;
	piece_list* last = NULL;
	while (p) {
		if (p->tile == tile) {
			if (last)  {
				last->next = p->next;
				break;
			} else
				return p->next;
		}
		last = p;
		p = p->next;
	}
	return head;
}

// Returns whether a tile is under attack by the opponent color
//   Technically not general, as doesn't include en passant attacks; meant for king
static int tile_attacked(game* g, int tile) {

	// Less dereferencing of board
	int* board = g->board;

	// King
	if (king_distances[tile][g->king_tiles[!COL_I(g->turn)]] == 1)
		return 1;

	// Knight
	for (int i = 0; i < 8; i++) {
		if ((board[knight_jumps[tile][i]] == knight) && (ENEMY_COLOR(board[knight_jumps[tile][i]], g->turn)))
			return 1;
	}

	// Pawn
	for (int i = 0; i < 2; i++) {
		if (tiles_from_edge[tile][pawn_capture_directions[COL_I(g->turn)][i]] > 0) {
			int capture_direction = directions[pawn_capture_directions[COL_I(g->turn)][i]];
			int destination = tile + capture_direction;
			int occupying = board[destination];
			if (ENEMY_COLOR(occupying, g->turn) && (PIECE_TYPE(occupying) == pawn))
				return 1;
		}
	}

	// Sliding pieces
	for (int di = 0; di < 8; di++) {
		for (int i = 0; i < tiles_from_edge[tile][di]; i++) {

			int destination = tile + directions[di] * (i + 1);
			int occupying = board[destination];

			if (SAME_COLOR(occupying, g->turn))
				break;

			// Check if occupying piece type is consistent with direction
			if (ENEMY_COLOR(occupying, g->turn)) {
				switch (PIECE_TYPE(occupying)) {
					case queen:
						return 1;
					case rook:
						if (di < 4)
							return 1;
						break;
					case bishop:
						if (di > 3)
							return 1;
						break;
				}
				break;
			}
		}
	}

	return 0;
}

static move* new_move(int start, int end, int captured, int promotion, int en_passant, int castle, move* next) {
	move* nm = malloc(sizeof(move));
	nm->start = start;
	nm->end = end;
	nm->captured = captured;
	nm->promotion = promotion;
	nm->en_passant = en_passant;
	nm->castle = castle;
	nm->next = next;
	return nm;
}

// Revokes the previous move
static void undo_move(game* g) {

	// Save most recent move
	move* prevm = g->moves_tail;

	// Fix move history
	move* ntail = g->moves_head;
	while (ntail) {
		if (ntail->next == prevm) {
			g->moves_tail = ntail;
			break;
		}
		ntail = ntail->next;
	}

	int piece = g->board[prevm->end];
	
	// Track kings
	if (PIECE_TYPE(piece) == king)
		g->king_tiles[COL_I(PIECE_COLOR(piece))] = prevm->start;

	// Create old piece (and depromote)
	if (prevm->promotion)
		g->board[prevm->start] = (PIECE_COLOR(piece) | pawn);
	else
		g->board[prevm->start] = piece;
	// Uncapture
	g->board[prevm->end] = prevm->captured;
	// En passant case (moves_tail->end is now the would-be location of the victim pawn)
	if (prevm->en_passant)
		g->board[g->moves_tail->end] = (PIECE_OCOLOR(piece) | pawn);
	
	// Move the rook when castling
	switch (prevm->castle) {
		// Right
		case 2:
			g->board[prevm->start + 3] = PIECE_COLOR(piece) | rook;
			g->board[prevm->end - 1] = 0;
			break;
		// Left
		case 1:
			g->board[prevm->start - 4] = PIECE_COLOR(piece) | rook;
			g->board[prevm->end + 1] = 0;
			break;
	}

}

// Returns a list of legal moves given a list of pseudo legal moves
static move* filter_legal_moves(game* g, move* m) {
	move* legal_head = NULL;
	move* legal_tail = NULL;

	while (m) {
		// Play each move
		make_move(g, m);
		// If king is not attacked, copy and add to legal moves
		if (!tile_attacked(g, g->king_tiles[COL_I(g->turn)])) {
			move* lm = new_move(m->start, m->end, m->captured, m->promotion, m->en_passant, m->castle, NULL);
			APPEND_LIST(legal_tail, legal_head, lm);
		}

		undo_move(g);
		m = m->next;
	}
	// Free pseudo-legal list
	while (m) {
		move* om = m;
		m = m->next;
		free(om);
	}

	return legal_head;
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
			if (jump_tile >= 0 && jump_tile < 64) {
				int jump_rank = jump_tile / 8;
				int jump_file = jump_tile % 8;
				int move_distance = max(abs(file - jump_file), abs(rank - jump_rank));
				if (move_distance == 2)
					knight_jumps[tile][i] = jump_tile;
				else
					knight_jumps[tile][i] = -1;
			}
		}

		// Calculate king moves
		for (int i = 0; i < 8; i++) {
			int move_tile = tile + directions[i];
			if (move_tile >= 0 && move_tile < 64) {
				int move_rank = move_tile / 8;
				int move_file = move_tile % 8;
				int move_distance = max(abs(file - move_file), abs(rank - move_rank));
				if (move_distance == 1)
					king_moves[tile][i] = move_tile;
				else
					king_moves[tile][i] = -1;
			}
		}
		
		// Calculate king distances
		for (int t2 = 0; t2 < 64; t2++) {
			int r2 = t2 / 8;
			int f2 = t2 % 8;
			king_distances[tile][t2] = max(abs(rank - r2), abs(file - f2));
		}
	}
}

// Makes a move
void make_move(game* g, move* m) {

	// Delete any captured piece from board
	int piece = g->board[m->start];
	int occupying = g->board[m->end];
	if (ENEMY_COLOR(piece, occupying)) {
		del_piece(g->pieces[!COL_I(piece)], m->end);
	}

	// Track king locations
	if (PIECE_TYPE(piece) == king)
		g->king_tiles[COL_I(g->turn)] = m->end;

	if (m->promotion) {
		// Create promoted piece
		g->board[m->end] = PIECE_TYPE(promotion_prompt()) | PIECE_COLOR(piece);
		g->board[m->start] = 0;
	} else {
		// Manage piece list/capturing for en passant
		if (m->en_passant) {
			g->board[g->moves_tail->end] = 0;
			del_piece(g->pieces[!COL_I(piece)], g->moves_tail->end);
		}

		// Move the rook when castling
		switch (m->castle) {
			// Right
			case 2:
				g->board[m->start + 3] = 0;
				g->board[m->end - 1] = PIECE_COLOR(piece) | rook;
				break;
			// Left
			case 1:
				g->board[m->start - 4] = 0;
				g->board[m->end + 1] = PIECE_COLOR(piece) | rook;
				break;
		}

		// Move the piece
		g->board[m->end] = piece;
		g->board[m->start] = 0;
	}

	// Add to move history
	if (g->moves_tail) {
		g->moves_tail->next = m;
		g->moves_tail = m;
	} else {
		g->moves_head = m;
		g->moves_tail = m;
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
		move* nm = new_move(tile, forward_tile, 0, next_promotion ? 1 : 0, 0, 0, NULL);
		APPEND_LIST(m, head, nm);
		if (rank == pawn_locations[c][0]) {
			int two_forward = forward_tile + forward;
			if (board[two_forward] == 0) {
				move* nm = new_move(tile, two_forward, 0, 0, 0, 0, NULL);
				APPEND_LIST(m, head, nm);
			}
		}
	}

	// Captures
	for (int i = 0; i < 2; i++) {
		if (tiles_from_edge[tile][pawn_capture_directions[c][i]] > 0) {
			int capture_direction = directions[pawn_capture_directions[c][i]];
			int destination = tile + capture_direction;
			int occupying = board[destination];
			if (ENEMY_COLOR(occupying, piece)) {
				move* nm = new_move(tile, destination, g->board[destination], next_promotion ? 1 : 0, 0, 0, NULL);
				APPEND_LIST(m, head, nm);

			// En passant
			} else if ((tile / 8) == pawn_locations[!c][3]) {
				move* last_move = g->moves_tail;
				if (two_pawn_push(last_move, board) && (last_move->end == destination + pawn_locations[!c][2])) {
					move* nm = new_move(tile, destination, 0, 0, 1, 0, NULL);
					APPEND_LIST(m, head, nm);
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

			if (SAME_COLOR(occupying, piece))
				break;

			move* nm = new_move(tile, destination, board[destination], 0, 0, 0, NULL);
			APPEND_LIST(m, head, nm);

			if (ENEMY_COLOR(occupying, piece))
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
		if SAME_COLOR(board[knight_jumps[tile][i]], piece)
			continue;

		move* nm = new_move(tile, knight_jumps[tile][i], board[knight_jumps[tile][i]], 0, 0, 0, NULL);
		APPEND_LIST(m, head, nm);
	}

	return head;
}

// Gets moves for a king
static move* get_king_moves(game* g, int tile) {
	move* m = NULL;
	move* head = NULL;
	int piece = g->board[tile];

	for (int i = 0; i < 8; i++) {
		if (king_moves[tile][i] == -1)
			continue;
		if (SAME_COLOR(g->board[king_moves[tile][i]], piece))
			continue;

		move* nm = new_move(tile, king_moves[tile][i], g->board[king_moves[tile][i]], 0, 0, 0, NULL);
		APPEND_LIST(m, head, nm);
	}

	// Castling:
	if ((!g->king_moved[COL_I(piece)]) && (!tile_attacked(g, tile))) {
		// Direction index, di = 3 is right, = 2 is left
		for (int di = 2; di < 4; di++) {
			// Rook moved index, 1 is right, 0 is left
			if (!g->rook_moved[COL_I(piece)][di - 2]) {
				for (int i = 0; i < 2; i++) {
					int destination = tile + directions[di] * (i + 1);

					if (g->board[destination] != 0)
						break;

					if (tile_attacked(g, destination))
						break;

					// Castle move property, 2 is right, 1 is left
					if (i == 1) {
						// Nothing between rook and king on left side
						if ((di == 2) && (g->board[destination - 1] != 0))
							break;

						move* nm = new_move(tile, destination, 0, 0, 0, di - 1, NULL);
						APPEND_LIST(m, head, nm);
					}
				}
			}
		}
	}

	return head;
}

// Gets moves for a specific piece
move* get_piece_moves(game* g, int tile) {
	switch (PIECE_TYPE(g->board[tile])) {
		case pawn:
			return filter_legal_moves(g, get_pawn_moves(g, tile));
		case knight:
			return filter_legal_moves(g, get_knight_moves(g->board, tile));
		case king:
			return filter_legal_moves(g, get_king_moves(g, tile));
		default:
			return filter_legal_moves(g, get_sliding_moves(g->board, tile));
	}
}

// Gets moves for a color index
move* get_moves(game* g, int c) {
	move* m = NULL;
	move* head = NULL;
	piece_list* p = g->pieces[c];
	while (p) {
		move* nm = get_piece_moves(g, p->tile);
		APPEND_LIST(m, head, nm);
		if (m)
			while (m->next) 
				m = m->next;
		p = p->next;
	}
	return head;
}

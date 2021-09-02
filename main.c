// Chess implemented in C; main.c is the entry point.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdio.h>

#include "game.h"

int main(int argc, char* argv[]) {
	compute_move_data();
	game ng = {
		.turn = white,
		.board = { 0 },
		.attack_map_white = { 0 },
		.attack_map_black = { 0 },
		.tiles_white = NULL,
		.tiles_black = NULL,
		.moves_head = NULL,
		.moves_tail = NULL,
		.ended = not_finished
	};
	game *g = &ng;
	load_fen(GAME_FEN, g);

	play(g);
}

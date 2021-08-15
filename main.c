// Chess implemented in C; main.c is the entry point.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdio.h>

#include "game.h"

int main(int argc, char* argv[]) {
	game ng = {
		.turn = white,
		.board = {0},
		.moves = NULL,
		.ended = not_finished
	};
	game *g = &ng;
	load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", g);
	play(g);
}

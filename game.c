// Chess implemented in C; game.c implements the game interface
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdio.h>

#include "game.h"

void render_board(int board[64]) {
	for (int i = 0; i < 64; i++) {
		printf("%c ", ptoc(board[i]));
		if (i % 8 == 7)
			printf("%s", "\n");
	}
}

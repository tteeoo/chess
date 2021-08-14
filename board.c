#include <stdio.h>

#include "board.h"

void render_board(int board[64]) {
	for (int i = 0; i < 64; i++) {
		printf("%c ", ptoc(board[i]));
		if (i % 8 == 7)
			printf("%s", "\n");
	}
}

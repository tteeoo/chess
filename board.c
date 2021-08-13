#include <stdio.h>

#include "board.h"

void render_board(board* b) {
	for (int i = 0; i < 64; i++) {
		printf("%c ", ptoc((*b)[i]));
		if (i % 8 == 7)
			printf("%s", "\n");
	}
}

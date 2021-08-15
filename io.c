// Chess implemented in C; io.c implements the user interface.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdio.h>

#include "game.h"

static int notation_to_tile(char notation[2][2]) {
	// TODO: figure this out
}

void render_board(int board[64]) {
	for (int i = 0; i < 64; i++) {
		printf("%c ", ptoc(board[i]));
		if (i % 8 == 7)
			printf("%s", "\n");
	}
}

void repl(game* g) {
	char prompt[COMMAND_LEN];
	sprintf(prompt, "%s to move: ", (g->turn == white) ? "white" : "black");
	while (1) {
		char command[COMMAND_LEN];
		printf("%s", prompt);
		fgets(command, COMMAND_LEN, stdin);
		
		// TODO: command parsing
	}
}

void play(game* g) {
	while (g->ended == not_finished) {
		render_board(g->board);
		repl(g);
		g->turn = (g->turn == white) ? black : white;
	}
	switch (g->ended) {
		case by_checkmate:
			printf("game ended: %s wins by checkmate\n", (g->turn == white) ? "black" : "white");
			break;
		case by_stalemate:
			printf("game ended: %s\n", "draw by stalemate");
			break;
		case by_repetition:
			printf("game ended: %s\n", "draw by threefold repetition");
			break;
		case by_fifty_move:
			printf("game ended: %s\n", "draw by fifty-move rule");
			break;
		default:
			break;
	}
}

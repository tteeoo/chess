// Chess implemented in C; io.c implements the user interface.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdio.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "game.h"

static int bad_notation(char notation[2]) {
	if (notation[0] < 'a' || notation[0] > 'h')
		return 1;
	if (notation[1] < '1' || notation[1] > '8')
		return 1;
	return 0;
}

static int notation_to_tile(char notation[2]) {
	int file = (int)(notation[0] - 'a');
	int rank = (int)(notation[1] - '1');
	return rank * 8 + file;
}

void render_board(int board[64], int do_highlight, int highlights[64]) {
	char board_buffer[8][17];
	int rank = 7;
	int file = 0;
	for (int i = 0; i < 64; i++) {
		board_buffer[rank][file] = ptoc(board[i]);
		if (do_highlight && highlights[i]) {
			if (board[i] != 0)
				board_buffer[rank][file] = 'x';
			else
				board_buffer[rank][file] = '*';
		}
		board_buffer[rank][file+1] = ' ';
		file += 2;
		if (i % 8 == 7) {
			board_buffer[rank][file] = '\0';
			file = 0;
			rank--;
		}
	}
	printf("\n");
	for (int i = 0; i < 8; i++) {
		printf("%s\n", board_buffer[i]);
	}
	printf("\n");
}

void repl(game* g) {
	int selected_tile = -1;
	int do_highlight = 0;
	int highlights[64] = {0};
	char prompt[PROMPT_LEN];
	sprintf(prompt, "%s to move: ", (g->turn == white) ? "WHITE" : "black");
	while (1) {
		char* command = readline(prompt);
	
		if (strlen(command) == 1) {
			switch (command[0]) {
				case 'b':
					render_board(g->board, do_highlight, highlights);
					continue;
				case 'n':
					selected_tile = -1;
					do_highlight = 0;
					sprintf(prompt, "%s to move: ", (g->turn == white) ? "WHITE" : "black");
					continue;
			}
		}
		if (strlen(command) == 2) {
			if (bad_notation(command)) {
				printf("bad notation\n");
				continue;
			}
			selected_tile = notation_to_tile(command);
			if (g->board[selected_tile] == 0 || !HAS_MASK(g->board[selected_tile], g->turn)) {
				printf("bad tile\n");
				selected_tile = -1;
				continue;
			}
				
			do_highlight = 1;
			move* m = get_piece_moves(g->board, selected_tile);

			if (!m) {
				printf("piece has no moves\n");
				do_highlight = 0;
				selected_tile = -1;
				continue;
			}

			sprintf(prompt, "%s to move (%s): ", (g->turn == white) ? "WHITE" : "black, command", command);
			while (m) {
				highlights[m->end] = 1;
				m = m->next;
			}
			render_board(g->board, do_highlight, highlights);
		}
	}
}

void play(game* g) {
	while (g->ended == not_finished) {
		render_board(g->board, 0, NULL);
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
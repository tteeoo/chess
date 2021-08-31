// Chess implemented in C; io.c implements the user interface.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "game.h"

// Returns 1 if the provided algebraic notation is invalid
static int bad_notation(char notation[2]) {
	if (notation[0] < 'a' || notation[0] > 'h')
		return 1;
	if (notation[1] < '1' || notation[1] > '8')
		return 1;
	return 0;
}

// Returns the index of a notated tile
static int notation_to_tile(char notation[2]) {
	int file = (int)(notation[0] - 'a');
	int rank = (int)(notation[1] - '1');
	return rank * 8 + file;
}

// Returns the notation of a tile index
static char* tile_to_notation(int tile) {
	int file = tile % 8;
	int rank = tile / 8;
	char* notation = malloc(3);
	sprintf(notation, "%c%c", 'a' + file, '1' + rank);
	return notation;
}

// Prints the board with move highlights
void render_board(int board[64], move* m) {
	char board_buffer[8][17];
	int rank = 7;
	int file = 0;
	for (int i = 0; i < 64; i++) {
		board_buffer[rank][file] = ptoc(board[i]);
		board_buffer[rank][file+1] = ' ';
		file += 2;
		if (i % 8 == 7) {
			board_buffer[rank][file] = '\0';
			file = 0;
			rank--;
		}
	}

	// Highlight moves
	if (m) {
		while (m) {
			rank = 7 - (m->end / 8);
			file = (m->end % 8) * 2;
			if (board[m->end] != 0)
				board_buffer[rank][file] = 'x';
			else if (m->en_passant)
				board_buffer[rank][file] = 'x';
			else
				board_buffer[rank][file] = '*';
			m = m->next;
		}
	}

	printf("\n   a b c d e f g h\n\n");
	for (int i = 0; i < 8; i++) {
		printf("%c  %s %c\n", '8' - i, board_buffer[i], '8' - i);
	}
	printf("\n   a b c d e f g h\n\n");
}

int promotion_prompt() {
	char* command;
	int piece;
	while (1) {
		command = readline("choose promotion (q/b/n/r) : ");
		if (command && *command)
			add_history(command);
		if (strlen(command) != 1) {
			printf("bad promotion\n");
			continue;
		}
		piece = ctop(command[0]);
		if ((piece == 0) || (PIECE_TYPE(piece) == pawn) || (PIECE_TYPE(piece) == king)) {
			printf("bad promotion\n");
			continue;
		}
		return piece;
	}
}

// Creates a read-evaluate-print loop until a move is made
void repl(game* g) {
	int selected_tile = -1;
	char prompt[PROMPT_LEN];
	sprintf(prompt, "%s to move : ", (g->turn == white) ? "WHITE" : "black");

	// Repeat until move is chosen
	while (1) {
		char* command = readline(prompt);
		if (command && *command)
			add_history(command);

		// One character commands
		if (strlen(command) == 1) {
			move* m = g->moves_head;
			int i = 1;
			switch (command[0]) {
				// Render board
				case 'b':
					render_board(g->board, NULL);
					continue;
				// Cancel selection
				case 'c':
					selected_tile = -1;
					sprintf(prompt, "%s to move : ", (g->turn == white) ? "WHITE" : "black");
					continue;
				// Move history
				case 'm':
					while (m) {
						printf("%d. %s%s", i, tile_to_notation(m->start), tile_to_notation(m->end));
						m = m->next;
						if (m) {
							printf(" %s%s\n", tile_to_notation(m->start), tile_to_notation(m->end));
							m = m->next;
						} else {
							printf("\n");
							break;
						}
						i++;
					}
					continue;
				// Quit
				case 'q':
					printf("quitting...\n");
					while (m) {
						move* om = m;
						m = m->next;
						free(om);
					}
					exit(0);
					continue;
			}
		}

		// Select a piece
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
				
			move* m = get_piece_moves(g, selected_tile);
			if (!m) {
				printf("piece has no moves\n");
				selected_tile = -1;
				continue;
			}

			sprintf(prompt, "%s to move (%s) : ", (g->turn == white) ? "WHITE" : "black", command);
			render_board(g->board, m);
			while (m) {
				move* om = m;
				m = m->next;
				free(om);
			}
			continue;
		}

		// Move is made
		if (strlen(command) == 4) {

			// Get piece moves
			char start[2] = {command[0], command[1]};
			char end[2] = {command[2], command[3]};
			if (bad_notation(start) || bad_notation(end)) {
				printf("bad notation\n");
				continue;
			}
			int start_tile = notation_to_tile(start);
			int end_tile = notation_to_tile(end);
			if (g->board[start_tile] == 0 || !HAS_MASK(g->board[start_tile], g->turn)) {
				printf("bad source tile\n");
				continue;
			}
			move* m = get_piece_moves(g, start_tile);

			// Check input move and make it
			while (m) {
				if (m->end == end_tile) {
					make_move(g, m);
					return;
				}
				move* om = m;
				m = m->next;
				free(om);
			}
			printf("bad move\n");
			continue;
		}
		printf("bad command\n");
	}
}

// Starts the game
void play(game* g) {

	// Take commands
	while (g->ended == not_finished) {
		render_board(g->board, NULL);
		repl(g);
		g->turn = (g->turn == white) ? black : white;
	}

	// Handle endings
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

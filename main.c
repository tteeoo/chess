#include <stdio.h>

#include "board.h"

int main(int argc, char* argv[]) {
	int board[64] = {0};
	load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", board);
	render_board(board);
}

#include <stdio.h>

#include "board.h"

int main(int argc, char* argv[]) {
	game g;
	load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", g);
	render_board(g.board);
}

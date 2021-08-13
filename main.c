#include <stdio.h>

#include "board.h"

int main(int argc, char* argv[]) {
	board b = {0};
	load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", b);
	render_board(b);
}

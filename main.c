#include <stdio.h>

#include "board.h"

int main(int argc, char* argv[]) {
	board* b = parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	render_board(b);
}

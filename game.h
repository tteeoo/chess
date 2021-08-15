// Chess implemented in C; game.h defines macros, types,
// and function signatures used throughout the program.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#define COMMAND_LEN 256
#define MIN(x, y) ((x > y) ? y : x)
#define HAS_MASK(piece, mask) ((piece & mask) == mask)
#define PIECE_TYPE(piece) (piece & ~(white | black))
#define PIECE_COLOR(piece) (piece & ~(pawn | knight | bishop | rook | queen | king))

#define PIECE_OCOLOR(piece) ((PIECE_COLOR(piece) == white) \
	? black : ((PIECE_COLOR(piece) == black) ? white : no_color))

#define SLIDING_PIECE(piece) \
	((PIECE_TYPE(piece) == queen) || \
		(PIECE_TYPE(piece) == bishop) || \
	(PIECE_TYPE(piece) == rook))

enum piece_color {
	black = 8,
	white = 16,
	no_color = 0
};

enum piece_type {
	pawn = 0,
	knight = 1,
	bishop = 2,
	rook = 3,
	queen = 4,
	king = 5
};

enum end_condition {
	not_finished,
	by_checkmate,
	by_stalemate,
	by_repetition,
	by_fifty_move
};

struct move {
	int start;
	int end;
	struct move* next;
};
typedef struct move move;

struct {
	enum piece_color turn;
	int board[64];
	move* moves;
	enum end_condition ended;
} typedef game;

void render_board(int[64]);
void repl(game*);
void play(game*);

void load_fen(char*, game*);
char ptoc(int);
int ctop(char);

void compute_move_data();
move* get_sliding_moves(int[64], int);
move* get_moves(game*);

// Chess implemented in C; game.h defines macros, types,
// and function signatures used throughout the program.
// Copyright (C) 2021 Theo Henson.
// Released under the GPL v3.0, see LICENSE.

#define GAME_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
#define PROMPT_LEN 256
#define HAS_MASK(piece, mask) ((piece & mask) == mask)
#define PIECE_TYPE(piece) (piece & ~(white | black))
#define PIECE_COLOR(piece) (piece & ~(pawn | knight | bishop | rook | queen | king))
#define PIECE_OCOLOR(piece) ((PIECE_COLOR(piece) == white) ? black : white)

#define SLIDING_PIECE(piece) \
	((PIECE_TYPE(piece) == queen) || \
		(PIECE_TYPE(piece) == bishop) || \
	(PIECE_TYPE(piece) == rook))

// The index for arrays pertaining to the color of a piece
#define COL_I(piece) ((HAS_MASK(piece, white)) ? 0 : 1)

// Appends an item to a linked list, initializing if necessary
#define APPEND_LIST(tail, head, add) { \
		if (tail) { \
			tail->next = add; \
			tail = add; \
		} else { \
			head = add; \
			tail = add; \
		} \
	}

enum piece_color {
	black = 8,
	white = 16
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
	int promotion;
	int en_passant;
	struct move* next;
};
typedef struct move move;

struct piece_list {
	int tile;
	struct piece_list* next;
};
typedef struct piece_list piece_list;

struct {
	enum piece_color turn;
	int board[64];
	int attack_map[2][64];
	piece_list* pieces[2];
	move* moves_head;
	move* moves_tail;
	enum end_condition ended;
} typedef game;

// io.c
void render_board(int[64], move* m);
int promotion_prompt();
void repl(game*);
void play(game*);

// fen.c
void load_fen(char*, game*);
char ptoc(int);
int ctop(char);

// moves.c
void compute_move_data();
void make_move(game*, move*);
move* get_piece_moves(game*, int);
move* get_moves(game*, int);

// check.c
void create_attack_map(game*);

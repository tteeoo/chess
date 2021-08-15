#define MIN(a, b) \
	({typeof (a) _a = (a); \
		typeof (b) _b = (b); \
	_a > _b ? _a : _b ;})

#define SLIDING_PIECE(piece) \
	((PIECE_TYPE(piece) == queen) || \
		(PIECE_TYPE(piece) == bishop) || \
	(PIECE_TYPE(piece) == rook))

#define HAS_MASK(piece, mask) ((piece & mask) == mask)
#define PIECE_TYPE(piece) (piece & ~(white | black))
#define PIECE_COLOR(piece) (piece & ~(pawn | knight | bishop | rook | queen | king))
#define PIECE_OCOLOR(piece) ((PIECE_COLOR(piece) == white) \
	? black : ((PIECE_COLOR(piece) == black) ? white : no_color))

enum color {
	black = 8,
	white = 16,
	no_color = 0
};

enum type {
	pawn = 0,
	knight = 1,
	bishop = 2,
	rook = 3,
	queen = 4,
	king = 5
};

struct move {
	int start;
	int end;
	struct move* next;
};
typedef struct move move;

struct {
	enum color turn;
	int board[64];
	move* moves;
} typedef game;

void render_board(int[64]);

void load_fen(char*, game*);
char ptoc(int);
int ctop(char);

void compute_move_data();
move* get_sliding_moves(int[64], int);
move* get_moves(game*);

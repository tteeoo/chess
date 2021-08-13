typedef unsigned char piece;
typedef unsigned char location;
typedef piece board[64];

enum color {
	black = 8,
	white = 16
};

enum type {
	pawn = 0,
	knight = 1,
	bishop = 2,
	rook = 3,
	queen = 4,
	king = 5
};

struct {
	location source;
	location destination;
} typedef move;

void load_fen(char*, board);
void render_board(board);
char ptoc(piece);
piece ctop(char);

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

void load_fen(char*, int[64]);
void render_board(int[64]);
char ptoc(int);
int ctop(char);

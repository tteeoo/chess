#define MIN(a, b) \
	({typeof (a) _a = (a); \
		typeof (b) _b = (b); \
	_a > _b ? _a : _b ;})

const int directions[] = { 8, -8, -1, 1, 7, -7, 9, -9 };

struct {
	int north;
	int south;
	int west;
	int east;
	int northwest;
	int northeast;
	int southwest;
	int southeast;
} typedef tiles;

struct move {
	int source;
	int destination;
	struct move* next;
};
typedef struct move move;

void load_move_data(tiles[64]);
// TODO 4:15
move* get_sliding_moves(int source, int p);
move* get_moves(int source, int p);

#include "moves.h"

void load_move_data(tiles tiles_from_edge[64]) {
	for (int file = 0; file < 8; file++) {
		for (int rank = 0; rank < 8; rank++) {
			int i = rank * 8 + file;
			int north = rank;
			int south = 7 - rank;
			int west = file;
			int east = 7 - file;
			tiles_from_edge[i].north = north;
			tiles_from_edge[i].south = south;
			tiles_from_edge[i].west = west;
			tiles_from_edge[i].east = east;
			tiles_from_edge[i].northwest = MIN(north, west);
			tiles_from_edge[i].northeast = MIN(north, east);
			tiles_from_edge[i].southwest = MIN(south, west);
			tiles_from_edge[i].southeast = MIN(south, east);
		}
	}
}

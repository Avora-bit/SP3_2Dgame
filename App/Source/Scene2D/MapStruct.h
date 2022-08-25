#pragma once

struct MapSize {
	unsigned int uiRowSize;
	unsigned int uiColSize;
};

// A structure storing information about a map grid
// It includes data to be used for A* Path Finding
struct Grid {
	unsigned int value;		//id of tile
	glm::vec2 pos;			//xy coord of tile
	glm::vec2 parent;		//a*
	unsigned int f;			//a*
	unsigned int g;			//a*
	unsigned int h;			//a*
	Grid()
		: value(0), pos(0, 0), parent(-1, -1), f(0), g(0), h(0) {}
	Grid(const glm::vec2& pos, unsigned int f)
		: value(0), pos(pos), parent(-1, 1), f(f), g(0), h(0) {}
	Grid(const glm::vec2& pos, const glm::vec2& parent,
		unsigned int f, unsigned int g, unsigned int h)
		: value(0), pos(pos), parent(parent), f(f), g(g), h(h) {}
};


//struct prefab {
//	unsigned int width;
//	unsigned int height;
//
//	int structure[width][height] = {
//	
//	
//	};
//
//};
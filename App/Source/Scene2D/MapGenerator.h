#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>

using namespace std;

struct Rect {
	int x, y;
	int width, height;
};

class MapGen {
private:
	int seed;
	int width = 100;
	int height = 100;

	vector<int>coremap;			//empty map
	vector<int>tempmap;			//temporary map for new generation

	//island cellular automata
	int birthlimit = 3;			//how many needed to create new cell
	int poplimit = 8;			//how many to die from over population
	int sociallimit = 4;		//how many to die from under population

	//dungeon 
	int roomChance = 60;
	int corridorChance = 100 - roomChance;

	int minRoomSize = 5;
	int maxRoomSize = 20;

	int minCorridorLength = 5;
	int maxCorridorLength = 10;

	vector<Rect> room; // rooms for place stairs or monsters
	vector<Rect> exit; // 4 sides of rooms or corridors

	enum Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		NUM_DIRECTIONS
	};

public:
	enum tiletype
	{
		//entities
		Player = 200,

		//background tiles
		Unused = 0,
		Void = 0,
		Water = 97,
		Grass = 99,
		Sand = 98,
		BrickFloor = 96,
		Trap = 95,
		Corridor = 96,

		//solid tiles
		Tree = 100,
		BrickWall = 101,

		//foreground tiles
		Cross = 80,
		DownStairs = 78,
		UpStairs = 77,
		Treasure = 79,
		Web = 76,

		//items
		Stick = 30,
		Wood = 40,
		Rock = 49,
		
	};

	MapGen() {
		//seeding
		seed = time(NULL);
		srand(seed);
	}
	~MapGen() {
		coremap.clear();
		tempmap.clear();
	}

	void createMap(int x, int y) {
		//erase
		coremap.clear();
		tempmap.clear();
		room.clear();
		exit.clear();
		//resize
		width = x; height = y;
		//generate
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				coremap.push_back(Unused);
			}
		}
	}

	void randomfill(tiletype typea, tiletype typeb, const float distribution = 0.5f) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int randchance = rand() % 100;
				if (randchance < (distribution * 100)) {
					coremap.at(i * width + j) = typea;
				}
				else {
					coremap.at(i * width + j) = typeb;
				}

			}
		}
	}

	void updateIsland() {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int type = Water; int neighbour = 0;
				switch (coremap.at(i * width + j))	//value of index
				{
				case Water:
					neighbour = countN((i * width + j), Grass);
					if (neighbour > birthlimit) {				//counts alive, if enough, born
						type = Grass;
					}
					break;
				case Grass:
					neighbour = countN((i * width + j), Grass);
					if (neighbour > poplimit) {					//counts other alive, if over populated, die
						type = Water;
					}
					else if (neighbour > sociallimit) {			//counts other alive, if enough, stay alive
						type = Grass;
					}
					else {										//else die
						type = Water;
					}
					break;
				default:
					//coremap.push_back(Void);
					break;
				}
				//temp storage assigned to new generation
				tempmap.push_back(type);
			}
		}

		coremap = tempmap;
		tempmap.clear();
	}

	void growtile(tiletype type) {
		//grow a tile from existing tiles
		tempmap = coremap;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (coremap.at(i * width + j) == Water &&			//if water
					(countN(i * width + j, Grass) > 0 ||
					countN(i * width + j, Sand) > 0)) {			//at least surrounded by land or sand
					tempmap.at(i * width + j) = type;
				}
			}
		}
		coremap = tempmap;
		tempmap.clear();
	}

	void placeRuins(vector<int> ruinarray, int row, int col) {
		bool checkPos = false;
		int randx = -1;
		int randy = -1;
		while (!checkPos) {
			//choose random position
			randx = rand() % width;
			randy = rand() % height;
			if (randx < 0 || randx >(width - (row + 1)) ||
				randy < 0 || randy >(height - (col + 1))) {		//bounds check
				continue;
			}
			else {
				//check if position has open 5x5 space
				vector<int> temp;
				for (int i = 0; i < col; i++) {
					for (int j = 0; j < row; j++) {
						//check from top left
						temp.push_back(Coord2Index(randx + j, randy + i));
					}
				}
				for (int i = 0; i < temp.size(); i++) {
					if (coremap.at(temp.at(i)) != Grass) {
						checkPos = false;
						break;
					}
					else {
						checkPos = true;
					}
				}
			}
		}
		//replace land with the structure
		std::cout << "Dungeon is at: " << randx << ", " << randy << std::endl;
		for (int i = 0; i < col; i++) {
			for (int j = 0; j < row; j++) {
				//check from top left
				coremap.at(Coord2Index(randx + i, randy + j)) = ruinarray[j + i * row];
			}
		}
	}

	char getTile(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= width || y >= height) return Unused;
		return coremap[x + y * width];
	}

	void setTile(int x, int y, char tiletype) {
		coremap[x + y * width] = tiletype;
	}

	bool createFeature() {
		for (int i = 0; i < 1000; ++i) {
			if (exit.empty()) break;

			// choose a random side of a random room or corridor
			int r = randminmax(0, exit.size());
			int x = randminmax(exit[r].x, exit[r].x + exit[r].width - 1);
			int y = randminmax(exit[r].y, exit[r].y + exit[r].height - 1);

			// north, south, west, east
			for (int j = 0; j < NUM_DIRECTIONS; ++j) {
				if (createFeature(x, y, static_cast<Direction>(j))) {
					exit.erase(exit.begin() + r);
					return true;
				}
			}
		}

		return false;
	}

	bool createFeature(int x, int y, Direction dir) {
		int dx = 0; int dy = 0;
		if (dir == UP) dy = 1;
		else if (dir == DOWN) dy = -1;
		else if (dir == LEFT) dx = 1;
		else if (dir == RIGHT) dx = -1;

		if (getTile(x + dx, y + dy) != BrickFloor && getTile(x + dx, y + dy) != Corridor)
			return false;

		if (randminmax(0, 100) < roomChance) {
			if (makeRoom(x, y, dir)) {
				setTile(x, y, Corridor);
				return true;
			}
		}
		else {
			if (makeCorridor(x, y, dir)) return true;
		}
		return false;
	}

	bool makeRoom(int x, int y, Direction dir, bool firstRoom = false) {
		Rect roomID;
		roomID.width = randminmax(minRoomSize, maxRoomSize);
		roomID.height = randminmax(minRoomSize, maxRoomSize);

		if (dir == UP) {
			roomID.x = x - roomID.width / 2;
			roomID.y = y - roomID.height;
		}
		else if (dir == DOWN) {
			roomID.x = x - roomID.width / 2;
			roomID.y = y + 1;
		}
		else if (dir == LEFT) {
			roomID.x = x - roomID.width;
			roomID.y = y - roomID.height / 2;
		}
		else if (dir == RIGHT) {
			roomID.x = x + 1;
			roomID.y = y - roomID.height / 2;
		}
		if (placeRect(roomID, BrickFloor)) {
			room.emplace_back(roomID);
			if (dir != DOWN || firstRoom) // north side
				exit.emplace_back(Rect{ roomID.x, roomID.y - 1, roomID.width, 1 });
			if (dir != UP || firstRoom) // south side
				exit.emplace_back(Rect{ roomID.x, roomID.y + roomID.height, roomID.width, 1 });
			if (dir != RIGHT || firstRoom) // west side
				exit.emplace_back(Rect{ roomID.x - 1, roomID.y, 1, roomID.height });
			if (dir != LEFT || firstRoom) // east side
				exit.emplace_back(Rect{ roomID.x + roomID.width, roomID.y, 1, roomID.height });

			return true;
		}

		return false;
	}

	bool makeCorridor(int x, int y, Direction dir)
	{
		Rect corridor;
		corridor.x = x;
		corridor.y = y;

		if (randBool()) // horizontal corridor
		{
			corridor.width = randminmax(minCorridorLength, maxCorridorLength);
			corridor.height = 1;

			if (dir == UP)
			{
				corridor.y = y - 1;

				if (randBool()) // west
					corridor.x = x - corridor.width + 1;
			}

			else if (dir == DOWN)
			{
				corridor.y = y + 1;

				if (randBool()) // west
					corridor.x = x - corridor.width + 1;
			}

			else if (dir == LEFT)
				corridor.x = x - corridor.width;

			else if (dir == RIGHT)
				corridor.x = x + 1;
		}

		else // vertical corridor
		{
			corridor.width = 1;
			corridor.height = randminmax(minCorridorLength, maxCorridorLength);

			if (dir == UP)
				corridor.y = y - corridor.height;

			else if (dir == DOWN)
				corridor.y = y + 1;

			else if (dir == LEFT)
			{
				corridor.x = x - 1;

				if (randBool()) // north
					corridor.y = y - corridor.height + 1;
			}

			else if (dir == RIGHT) {
				corridor.x = x + 1;

				if (randBool()) // north
					corridor.y = y - corridor.height + 1;
			}
		}

		if (placeRect(corridor, Corridor)) {
			if (dir != DOWN && corridor.width != 1) // north side
				exit.emplace_back(Rect{ corridor.x, corridor.y - 1, corridor.width, 1 });
			if (dir != UP && corridor.width != 1) // south side
				exit.emplace_back(Rect{ corridor.x, corridor.y + corridor.height, corridor.width, 1 });
			if (dir != RIGHT && corridor.height != 1) // west side
				exit.emplace_back(Rect{ corridor.x - 1, corridor.y, 1, corridor.height });
			if (dir != LEFT && corridor.height != 1) // east side
				exit.emplace_back(Rect{ corridor.x + corridor.width, corridor.y, 1, corridor.height });

			return true;
		}

		return false;
	}

	bool placeRect(const Rect& rect, tiletype type)
	{
		if (rect.x < 1 || rect.y < 1 || rect.x + rect.width > width - 1 || rect.y + rect.height > height - 1)
			return false;

		for (int y = rect.y; y < rect.y + rect.height; ++y) {
			for (int x = rect.x; x < rect.x + rect.width; ++x) {
				if (getTile(x, y) != Unused)
					return false; // the area already used
			}
		}

		for (int y = rect.y - 1; y < rect.y + rect.height + 1; ++y) {
			for (int x = rect.x - 1; x < rect.x + rect.width + 1; ++x) {
				if (x == rect.x - 1 || y == rect.y - 1 || x == rect.x + rect.width || y == rect.y + rect.height)
					setTile(x, y, BrickWall);
				else
					setTile(x, y, type);
			}
		}

		return true;
	}

	bool placeObject(tiletype type) {
		if (room.empty()) return false;

		int r = randminmax(0, room.size()); // choose a random room
		int x = randminmax(room[r].x + 1, room[r].x + room[r].width - 2);
		int y = randminmax(room[r].y + 1, room[r].y + room[r].height - 2);

		if (getTile(x, y) == BrickFloor) {
			setTile(x, y, type);
			//place one object in one room (optional)
			room.erase(room.begin() + r);
			return true;
		}
		return false;
	}
	void generateDungeon(int maxFeatures)
	{
		// place the first room in the center
		if (!makeRoom(width / 2, height / 2, static_cast<Direction>(randminmax(0, 3), true)))
		{
			std::cout << "Unable to place the first room.\n";
			return;
		}

		// we already placed 1 feature (the first room)
		for (int i = 1; i < maxFeatures; ++i)
		{
			if (!createFeature())
			{
				std::cout << "Unable to place more features (placed " << i << ").\n";
				break;
			}
		}
		//enter and exit
		if (!placeObject(UpStairs))
		{
			std::cout << "Unable to place up stairs.\n";
			return;
		}
		/*if (!placeObject(DownStairs))
		{
			std::cout << "Unable to place down stairs.\n";
			return;
		}*/
	}
	//randomly chooses a tile of typeX, to be replaced with
	void randreplace(tiletype replaced, tiletype type) {
		//gather all index with the same type
		for (int i = 0; i < coremap.size(); i++) {
			if (coremap.at(i) == type) {
				tempmap.push_back(i);
			}
		}
		if (tempmap.size() > 0) {			//if the type is available
			//choose random index
			int randpos = rand() % tempmap.size();
			//replace with key
			coremap.at(tempmap.at(randpos)) = replaced;
		}

		//clean up
		tempmap.clear();
	}

	void importmap(string filename) {
		coremap.clear();
		ifstream imported(filename);
		//ignore first row

		//check width and height of the map

		//push all tiles into new vector


		////first row
		//map << "//";
		//for (int i = 1; i <= width; i++) {
		//	map << i;
		//	if (i < width) {
		//		map << ",";
		//	}
		//}
		//map << std::endl;
		////actual map
		//for (int i = 0; i < height; i++) {
		//	for (int j = 0; j < width; j++) {
		//		map << coremap.at(i * width + j);
		//		if (j < width - 1) {
		//			map << ",";
		//		}
		//	}
		//	map << std::endl;
		//}
		////close file
		//map.close();


		//ifstream book(filename);		//input only
		////create array
		//string linetext;	Recipe tempRecipe;			//create new recipe
		//while (getline(book, linetext, ' ')) {		//space for carriage return, hardcoded ',' for delimiter
		//	// Output the text from the file
		//	int value, count = 0;	string token;  size_t pos;
		//	while ((pos = linetext.find(',')) != std::string::npos) {
		//		token = linetext.substr(0, pos);
		//		value = stoi(token);
		//		tempRecipe.SetRecipeIndex(count, value);
		//		count++;
		//		linetext.erase(0, pos + 1);		//delete index
		//	}
		//	count = 0;
		//	//push recipe
		//	recipeList.push_back(tempRecipe);
		//}
		//book.close();
	}

	void exportmap(string filename, int layer) {
		ofstream map(filename);
		//first row
		map << "//";
		for (int i = 1; i <= width; i++) {
			map << i;
			if (i < width) {
				map << ",";
			}
		}
		map << std::endl;
		//actual map
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				//convert map to foreground
				if (layer == 0) {		//background
					//remove all foreground items
					if (coremap.at(i * width + j) == Tree || coremap.at(i * width + j) == BrickWall ||
						coremap.at(i * width + j) == UpStairs || coremap.at(i * width + j) == DownStairs) {
						//base generation of map does not create entity instances
						map << coremap.at(Void);
					}
					else {
						map << coremap.at(i * width + j);
					}
				}
				else if (layer == 1) {
					//production code alr deletes all instances of background
					map << coremap.at(i * width + j);
					
					
				}
				else {}
				if (j < width - 1) {
					map << ",";
				}
			}
			map << std::endl;
		}
		//close file
		map.close();
	}

	//convert index into coords
	int getX(int index) { return index % width; }
	int getY(int index) { return (index - (index % width)) / width; }
	//convert coord to index
	int Coord2Index(int x, int y) { return x + (y * width); }

	int randminmax(int min, int max) {
		if (max > min)  return (rand() % (max - min)) + min;
		else return min;
	}

	bool randBool(int distribution = 50) {			//% closer to true
		return rand() % 100 < distribution;
	}

	int countN(int index, tiletype type) {		//counts the type of cells around itself
		int X = getX(index); int Y = getY(index);
		int Neighbour = 0;
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (((X + i) < 0) || ((X + i) > (width - 1)) ||
					((Y + j) < 0) || ((Y + j) > (height - 1))) {
					//check map boundary
					if (type == Void || type == Unused) {		//edge counts as 0
						Neighbour++;
					}
				}
				else if (i == 0 && j == 0) {
					//offset is 0,0, self cell
					//ignore
				}
				else {
					//do stuff
					if (coremap.at(Coord2Index(X + i, Y + j)) == type) {
						Neighbour++;
					}
				}
			}
		}
		return Neighbour;
	}

	void deleteall(tiletype type) {
		//replace tile with 0
		for (int i = 0; i < coremap.size(); i++) {
			if (coremap.at(i) == type) {
				coremap.at(i) = Void;
			}
		}
	}

	//debug only
	void printmap() {
		system("CLS");
		std::cout << seed << std::endl;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				std::cout << coremap.at(i * width + j);
			}
			std::cout << std::endl;
		}
	}

	int structure_ruins[5][5] = {
	{101,96, 101,96, 101},
	{101,96, 96, 96, 101},
	{101,96, 78, 96, 96},
	{101,96, 96, 96, 96},
	{96, 101,96, 101,101}
	};

};

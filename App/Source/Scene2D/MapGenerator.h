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

	vector<int>coremap;		//empty map
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

	vector<char> tile;
	vector<Rect> room; // rooms for place stairs or monsters
	vector<Rect> exit; // 4 sides of rooms or corridors

	int Key_Convert[13][2] = {
		//background tiles
		{0, 0},				//void
		{1, 97},			//water		//cannot dash, slows movement speed
		{2, 99},			//grass		//spawn tree
		{3, 98},			//sand		//spawn cross
		{4, 96},			//brick floor		//no behavior
		{5, 95},			//trap				//deals small amount of damage to the player, 5 hp
		//solid tiles
		{6, 100},			//tree
		{7, 101},			//brick wall
		//foreground tiles
		{8, 80},			//cross		//spawn treasure
		{9, 79},			//treasure	//spawn loot
		{10, 78},			//ladderdown
		{11, 77},			//ladderup
		{12, 76},			//web		//slows player

	};

	enum Tile
	{
		Unused = ' ',
		Floor = '.',
		Corridor = ',',
		Wall = '#',
		Door = '+',
		UpStairs = '<',
		DownStairs = '>'
	};

	enum Direction
	{
		North,
		South,
		West,
		East,
		DirectionCount
	};

	char getTile(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= width || y >= height)
			return Unused;

		return tile[x + y * width];
	}

	void setTile(int x, int y, char tiletype)
	{
		tile[x + y * width] = tiletype;
	}

	bool createFeature()
	{
		for (int i = 0; i < 1000; ++i)
		{
			if (exit.empty())
				break;

			// choose a random side of a random room or corridor
			int r = randminmax(0, exit.size());
			int x = randminmax(exit[r].x, exit[r].x + exit[r].width - 1);
			int y = randminmax(exit[r].y, exit[r].y + exit[r].height - 1);

			// north, south, west, east
			for (int j = 0; j < DirectionCount; ++j)
			{
				if (createFeature(x, y, static_cast<Direction>(j)))
				{
					exit.erase(exit.begin() + r);
					return true;
				}
			}
		}

		return false;
	}

	bool createFeature(int x, int y, Direction dir)
	{
		int dx = 0;
		int dy = 0;

		if (dir == North)
			dy = 1;
		else if (dir == South)
			dy = -1;
		else if (dir == West)
			dx = 1;
		else if (dir == East)
			dx = -1;

		if (getTile(x + dx, y + dy) != Floor && getTile(x + dx, y + dy) != Corridor)
			return false;

		if (randminmax(0, 100) < roomChance)
		{
			if (makeRoom(x, y, dir))
			{
				setTile(x, y, Door);

				return true;
			}
		}

		else
		{
			if (makeCorridor(x, y, dir))
			{
				if (getTile(x + dx, y + dy) == Floor)
					setTile(x, y, Door);
				else // don't place a door between corridors
					setTile(x, y, Corridor);

				return true;
			}
		}

		return false;
	}

	bool makeRoom(int x, int y, Direction dir, bool firstRoom = false)
	{
		Rect roomID;
		roomID.width = randminmax(minRoomSize, maxRoomSize);
		roomID.height = randminmax(minRoomSize, maxRoomSize);

		if (dir == North)
		{
			roomID.x = x - roomID.width / 2;
			roomID.y = y - roomID.height;
		}

		else if (dir == South)
		{
			roomID.x = x - roomID.width / 2;
			roomID.y = y + 1;
		}

		else if (dir == West)
		{
			roomID.x = x - roomID.width;
			roomID.y = y - roomID.height / 2;
		}

		else if (dir == East)
		{
			roomID.x = x + 1;
			roomID.y = y - roomID.height / 2;
		}

		if (placeRect(roomID, Floor))
		{
			room.emplace_back(roomID);

			if (dir != South || firstRoom) // north side
				exit.emplace_back(Rect{ roomID.x, roomID.y - 1, roomID.width, 1 });
			if (dir != North || firstRoom) // south side
				exit.emplace_back(Rect{ roomID.x, roomID.y + roomID.height, roomID.width, 1 });
			if (dir != East || firstRoom) // west side
				exit.emplace_back(Rect{ roomID.x - 1, roomID.y, 1, roomID.height });
			if (dir != West || firstRoom) // east side
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

			if (dir == North)
			{
				corridor.y = y - 1;

				if (randBool()) // west
					corridor.x = x - corridor.width + 1;
			}

			else if (dir == South)
			{
				corridor.y = y + 1;

				if (randBool()) // west
					corridor.x = x - corridor.width + 1;
			}

			else if (dir == West)
				corridor.x = x - corridor.width;

			else if (dir == East)
				corridor.x = x + 1;
		}

		else // vertical corridor
		{
			corridor.width = 1;
			corridor.height = randminmax(minCorridorLength, maxCorridorLength);

			if (dir == North)
				corridor.y = y - corridor.height;

			else if (dir == South)
				corridor.y = y + 1;

			else if (dir == West)
			{
				corridor.x = x - 1;

				if (randBool()) // north
					corridor.y = y - corridor.height + 1;
			}

			else if (dir == East)
			{
				corridor.x = x + 1;

				if (randBool()) // north
					corridor.y = y - corridor.height + 1;
			}
		}

		if (placeRect(corridor, Corridor))
		{
			if (dir != South && corridor.width != 1) // north side
				exit.emplace_back(Rect{ corridor.x, corridor.y - 1, corridor.width, 1 });
			if (dir != North && corridor.width != 1) // south side
				exit.emplace_back(Rect{ corridor.x, corridor.y + corridor.height, corridor.width, 1 });
			if (dir != East && corridor.height != 1) // west side
				exit.emplace_back(Rect{ corridor.x - 1, corridor.y, 1, corridor.height });
			if (dir != West && corridor.height != 1) // east side
				exit.emplace_back(Rect{ corridor.x + corridor.width, corridor.y, 1, corridor.height });

			return true;
		}

		return false;
	}

	bool placeRect(const Rect& rect, char tile)
	{
		if (rect.x < 1 || rect.y < 1 || rect.x + rect.width > width - 1 || rect.y + rect.height > height - 1)
			return false;

		for (int y = rect.y; y < rect.y + rect.height; ++y)
			for (int x = rect.x; x < rect.x + rect.width; ++x)
			{
				if (getTile(x, y) != Unused)
					return false; // the area already used
			}

		for (int y = rect.y - 1; y < rect.y + rect.height + 1; ++y)
			for (int x = rect.x - 1; x < rect.x + rect.width + 1; ++x)
			{
				if (x == rect.x - 1 || y == rect.y - 1 || x == rect.x + rect.width || y == rect.y + rect.height)
					setTile(x, y, Wall);
				else
					setTile(x, y, tile);
			}

		return true;
	}

	bool placeObject(char tile)
	{
		if (room.empty())
			return false;

		int r = randminmax(0, room.size()); // choose a random room
		int x = randminmax(room[r].x + 1, room[r].x + room[r].width - 2);
		int y = randminmax(room[r].y + 1, room[r].y + room[r].height - 2);

		if (getTile(x, y) == Floor)
		{
			setTile(x, y, tile);

			//place one object in one room (optional)
			room.erase(room.begin() + r);

			return true;
		}

		return false;
	}

public:
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
		//resize
		width = x; height = y;
		//generate
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				coremap.push_back(0);
			}
		}
	}

	void randomfill(int typea, int typeb, const float distribution = 0.5f) {
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
				int type = 1; int neighbour = 0;
				switch (coremap.at(i * width + j))	//value of index
				{
				case 1:
					neighbour = countN((i * width + j), 2);
					if (neighbour > birthlimit) {				//counts alive, if enough, born
						type = 2;
					}
					break;
				case 2:
					neighbour = countN((i * width + j), 2);
					if (neighbour > poplimit) {					//counts other alive, if over populated, die
						type = 1;
					}
					else if (neighbour > sociallimit) {			//counts other alive, if enough, stay alive
						type = 2;
					}
					else {										//else die
						type = 1;
					}
					break;
				default:
					//coremap.push_back(0);
					break;
				}
				//temp storage assigned to new generation
				tempmap.push_back(type);
			}
		}

		coremap = tempmap;
		tempmap.clear();
	}

	void growtile(int type) {
		//grow a tile from existing tiles
		tempmap = coremap;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (coremap.at(i * width + j) == 1 &&			//if water
					(countN(i * width + j, 2) > 0 ||
						countN(i * width + j, 3) > 0)) {			//at least surrounded by land or sand
					tempmap.at(i * width + j) = type;
				}
			}
		}
		coremap = tempmap;
		tempmap.clear();
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

		if (!placeObject(UpStairs))
		{
			std::cout << "Unable to place up stairs.\n";
			return;
		}

		if (!placeObject(DownStairs))
		{
			std::cout << "Unable to place down stairs.\n";
			return;
		}

		for (char& tiletype : tile)
		{
			if (tiletype == Unused)
				tiletype = '.';
			else if (tiletype == Floor || tiletype == Corridor)
				tiletype = ' ';
		}
	}

	void print()
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
				std::cout << getTile(x, y);

			std::cout << std::endl;
		}
	}

	void updateDungeon() {
		//Dungeon d(100, 100);	//size of the map
		//d.generate(100);		//max features
		//d.print();
	}

	//randomly chooses a tile of typeX, to be replaced with
	void randreplace(int replaced, int type) {
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

	void exportmap(string filename) {
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
				map << coremap.at(i * width + j);
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

	int countN(int index, int type) {		//counts the type of cells around itself
		int X = getX(index); int Y = getY(index);
		int Neighbour = 0;
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (((X + i) < 0) || ((X + i) > (width - 1)) ||
					((Y + j) < 0) || ((Y + j) > (height - 1))) {
					//check map boundary
					if (type == 0) {		//edge counts as 0
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

	void convertKeys() {
		for (int i = 0; i < coremap.size(); i++) {
			for (int j = 0; j < size(Key_Convert); j++) {
				if (coremap.at(i) == Key_Convert[j][0]) {
					coremap.at(i) = Key_Convert[j][1];
					break;
				}
			}
		}
	}

	void revertKeys() {
		for (int i = 0; i < coremap.size(); i++) {
			for (int j = 0; j < size(Key_Convert); j++) {
				if (coremap.at(i) == Key_Convert[j][1]) {
					coremap.at(i) = Key_Convert[j][0];
					break;
				}
			}
		}
	}

	void deleteall(int type) {
		//replace tile with 0
		for (int i = 0; i < coremap.size(); i++) {
			if (coremap.at(i) == type) {
				coremap.at(i) = 0;
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
};
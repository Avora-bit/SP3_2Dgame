#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>

using namespace std;

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

	void updateDungeon() {
		//rectangular features
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
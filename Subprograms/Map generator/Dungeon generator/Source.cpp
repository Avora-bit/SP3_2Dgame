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
	vector<int>dungeonmap;		//empty map
	vector<int>tempmap;			//temporary map for new generation
	int birthlimit = 3;			//how many needed to create new cell
	int poplimit = 8;			//how many to die from over population
	int sociallimit = 4;		//how many to die from under population

	int Key_Convert[7][2] = {
		//background tiles
		{0, 97},			//water
		{1, 99},			//grass		//spawn tree
		{2, 98},			//sand		//spawn cross
		{4, 96},			//cross		//spawn treasure
		{5, 95},			//treasure
		//solid tiles
		{6, 100},			//wall
		{7, 101},			//tree
	};

public:
	MapGen() {
		//seeding
		seed = time(NULL);
		srand(seed);
	}
	~MapGen() {
		dungeonmap.clear();
		tempmap.clear();
	}

	void createMap(int x, int y) {
		//erase
		dungeonmap.clear();
		//resize
		width = x; height = y;
		//generate
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				dungeonmap.push_back(0);
			}
		}
	}

	void randomfill(void) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				dungeonmap.at(i * width + j) = rand() % 2;
			}
		}
	}

	void updateMap() {
		//update
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int type = 0; int neighbour = 0;
				switch (dungeonmap.at(i * width + j))	//value of index
				{
				case 0:
					neighbour = countN((i * width + j), 1);
					if (neighbour > birthlimit) {				//counts alive, if enough, born
						type = 1;
					}
					break;
				case 1:
					neighbour = countN((i * width + j), 1);
					if (neighbour > poplimit) {					//counts other alive, if over populated, die
						type = 0;
					}
					else if (neighbour > sociallimit) {			//counts other alive, if enough, stay alive
						type = 1;
					}
					else {										//else die
						type = 0;
					}
					break;
				default:
					//dungeonmap.push_back(0);
					break;
				}
				//temp storage assigned to new generation
				tempmap.push_back(type);
			}
		}
		dungeonmap = tempmap;
		tempmap.clear();
	}

	void growsand() {
		//update
		tempmap = dungeonmap;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int type = 2;
				if (dungeonmap.at(i * width + j) == 0 &&			//if water
					(countN(i * width + j, 1) > 0 ||
						countN(i * width + j, 2) > 0)) {			//at least surrounded by land or sand
					//grow sand
					tempmap.at(i * width + j) = type;
				}
			}
		}
		dungeonmap = tempmap;
		tempmap.clear();
	}
	//randomly chooses a tile of typeX, to be replaced with
	void randreplace(int replaced, int type) {
		//gather all index with the same type
		for (int i = 0; i < dungeonmap.size(); i++) {
			if (dungeonmap.at(i) == type) {
				tempmap.push_back(i);
			}
		}
		if (tempmap.size() > 0) {			//if the type is available
			//choose random index
			int randpos = rand() % tempmap.size();
			//replace with key
			dungeonmap.at(tempmap.at(randpos)) = replaced;
		}

		//clean up
		tempmap.clear();
	}

	//debug only
	void printmap() {
		system("CLS");
		std::cout << seed << std::endl;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				std::cout << dungeonmap.at(i * width + j);
			}
			std::cout << std::endl;
		}
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
				map << dungeonmap.at(i * width + j);
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
					if (dungeonmap.at(Coord2Index(X + i, Y + j)) == type) {
						Neighbour++;
					}
				}
			}
		}
		return Neighbour;
	}

	void convertKeys() {
		for (int i = 0; i < dungeonmap.size(); i++) {
			for (int j = 0; j < size(Key_Convert); j++) {
				if (dungeonmap.at(i) == Key_Convert[j][0]) {
					tempmap.push_back(Key_Convert[j][1]);
				}
			}
		}
		dungeonmap = tempmap;
		tempmap.clear();
	}
};

//int main(void) {
//	MapGen* Dmap = new MapGen;
//	Dmap->createMap(CSettings::GetInstance()->NUM_TILES_XAXIS,
//		CSettings::GetInstance()->NUM_TILES_YAXIS);
//	Dmap->randomfill();
//	for (int i = 0; i < 20; i++) {				//rounding out edges
//		Dmap->updateMap();
//	}
//	Dmap->growsand();		//sand radius of 1
//	//replace proper keys
//	Dmap->convertKeys();
//	Dmap->randreplace(200, 98);			//replace sand with player
//
//	int randspawn = rand() % 5 + 5;		//random number of chests, 5-10 chests
//	for (int i = 0; i < randspawn; i++) {
//		Dmap->randreplace(96, 98);			//replace sand with cross
//	}
//
//	randspawn = rand() % 20 + 20;		//random number of chests, 20-40 trees
//	for (int i = 0; i < randspawn; i++) {
//		Dmap->randreplace(96, 98);			//replace grass with tree
//	}
//
//	string filename = "Maps/Map.csv";
//	Dmap->exportmap(filename);
//	//clean
//	delete Dmap;
//	Dmap = nullptr;
//}
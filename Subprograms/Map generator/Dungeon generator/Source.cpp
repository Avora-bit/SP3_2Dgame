#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>

using namespace std;

class map {
private:
	int seed;
	int width = 100;
	int height = 100;
	vector<int>dungeonmap;		//empty map
	vector<int>tempmap;			//temporary map for new generation
	int birthlimit = 3;			//how many needed to create new cell
	int poplimit = 8;			//how many to die from over population
	int sociallimit = 4;		//how many to die from under population

public:
	map() {
		//seeding
		seed = time(NULL);
		srand(seed);
	}
	~map() {
		dungeonmap.clear();
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
					countN(i * width + j, 2) > 0)) {					//at least surrounded by land or sand
					//grow sand
					tempmap.at(i * width + j) = type;
				}
			}
		}
		dungeonmap = tempmap;
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
			map << i << ",";
		}
		map << std::endl;
		//actual map
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				map << dungeonmap.at(i * width + j) << ",";
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

	void convertKey(int index, int state, int key) {
		if (dungeonmap.at(index) == state) {
			dungeonmap.at(index) = key;
		}
	}
};

int main(void) {

	//convert all instances into map values
	//index 0 = original tile
	//index 1 = replaced tile
	int Key_Convert[6][2] = { 
		//background tiles
		{0, 0},						//void
		{1, 1},						//water		//swim
		{2, 2},						//sand		//spawn treasure cross
		{3, 3},						//grass		//trees
		//solid tiles
		{4, 100},					//wall
	};


	//place player
	//choose random open spot, replace tile with player


	map* Dmap = new map;
	Dmap->createMap(50, 50);
	Dmap->randomfill();
	Dmap->printmap();
	for (int i = 0; i < 30; i++) {
		Dmap->updateMap();
		Dmap->printmap();
	}
	for (int i = 0; i < 1; i++) {	//loop for size of sand
		Dmap->growsand();
		Dmap->printmap();
	}

	string filename = "Maps/Map.csv";
	Dmap->exportmap(filename);

	delete Dmap;
	Dmap = nullptr;
	
	return 0;
}


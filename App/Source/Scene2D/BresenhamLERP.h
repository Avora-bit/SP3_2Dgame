#pragma once

#include <iostream>
#include <vector>

using namespace std;

class Blerp {
	//using 2 points on a line, extrapolate data to check line of sight
	//2 coords, get gradient
	//split gradient to find out which grids are empty
	//thus achieving line of sight
private:
	int originCoord[2], targetCoord[2];
	float gradient;

	vector<int> opengrid;

public:
	Blerp(int originX, int originY, int targetX, int targetY) {
		originCoord[0] = originX; originCoord[1] = originX;
		targetCoord[0] = targetX; targetCoord[1] = targetY;
		gradient = (targetY - originY) / (targetX - originX);
	}
	~Blerp() {

	}
	void LERPing() {

	}
};
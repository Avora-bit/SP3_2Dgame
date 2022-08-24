#pragma once

//include dependencies
#include <iostream>

//include entities
#include "Map2D.h"
#include "Player2D.h"
#include "Enemy2D.h"
//#include "item.h"


using namespace std;

class EventController {
private:
	//vectors to store data

	//enemy
	//item
	//event

public:
	EventController() {

	}

	~EventController() {

	}

	void Init() {
		//creates all instances
	}

	void spawnenemies() {
		cout << "enemyspawn" << endl;
	}

	void spawnitems() {
		cout << "itemspawn" << endl;
	}

	void events() {			//controls events
		cout << "eventstarted" << endl;
	}


	void update() {			//runs every frame
		cout << "eventupdate" << endl;
	}
};

#pragma once

//include dependencies
#include <iostream>

//include entities
//foreground and background
//player instance
#include "Player2D.h"
#include "Octopus.h"
#include "Chicken.h"
#include "Spider.h"
//#include "item.h"


using namespace std;

class EventController : public CSingletonTemplate<EventController> {
	friend CSingletonTemplate<EventController>;
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

	vector<CEntity2D*> enemyVector;
	vector<CEntity2D*> projectileVector;

	void Init() {
		//creates all instances
		enemyVector.clear();
		projectileVector.clear();
	}

	void spawnEnemies(CEnemy2D* enemy) {
		enemyVector.push_back(enemy);
	}

	void spawnitems() {
		cout << "itemspawn" << endl;
	}

	void events() {			//controls events
		cout << "eventstarted" << endl;
	}


	void update(const double dElapsedTime) {			//runs every frame
		for (int i = 0; i < enemyVector.size(); i++)
		{
			enemyVector[i]->Update(dElapsedTime);
		}

		for (int i = 0; i < projectileVector.size(); i++)
		{
			projectileVector[i]->Update(dElapsedTime);
		}
	}
};

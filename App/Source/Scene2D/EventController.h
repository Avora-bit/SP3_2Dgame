#pragma once

//include dependencies
#include <iostream>

//include entities
//foreground and background
//player instance
#include "Map2D.h"
#include "Player2D.h"
//#include "Shivs2D.h"

#include "Octopus.h"
#include "Chicken.h"
#include "Spider.h"

using namespace std;

class EventController : public CSingletonTemplate<EventController> {
	friend CSingletonTemplate<EventController>;
private:

public:
	EventController() {

	}

	~EventController() {
	
	}

	vector<CEntity2D*> enemyVector;
	vector<CEntity2D*> projectileVector;

	void Init() {
		//get player and map singleton

		//creates all instances
		enemyVector.clear();
		projectileVector.clear();
	}

	void spawnEnemies(CEntity2D* enemy) {
		enemyVector.push_back(enemy);
	}

	void spawnProjectiles(CEntity2D* projectile/*, glm::vec2 position, glm::vec2 direction*/) {
		cout << "projectilespawn" << endl;
		projectileVector.push_back(projectile);
	}

	void spawnitems(int itemID/*, location*/) {
		cout << "itemspawn" << endl;
	}

	void update(const double dElapsedTime) {
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

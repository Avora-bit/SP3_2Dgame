#pragma once

//include dependencies
#include <iostream>

//include entities
//foreground and background
//player instance
#include "Map2D.h"
#include "Player2D.h"
#include "Projectile2D.h"
//#include "item.h"

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

	void spawnProjectiles(CProjectile2D* projectile, glm::vec2 vec2Index) {
		projectile->vec2Index = vec2Index;
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

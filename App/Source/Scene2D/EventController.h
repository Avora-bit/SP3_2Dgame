#pragma once

//include dependencies
#include <iostream>

//include entities
//foreground and background
//player instance
#include "Map2D.h"
#include "Enemy2D.h"
#include "Projectile2D.h"

using namespace std;

class EventController : public CSingletonTemplate<EventController> {
	friend CSingletonTemplate<EventController>;
private:

public:
	EventController() {

	}

	~EventController() {
	
	}

	vector<CEnemy2D*> enemyVector; // existing entities
	vector<CEntity2D*> projectileVector;

	void Init() {
		//creates all instances
		enemyVector.clear();
		projectileVector.clear();
	}

	void spawnEnemies(CEnemy2D* enemy) {
		enemyVector.push_back(enemy);
	}

	void spawnProjectiles(CProjectile2D* projectile, glm::vec2 vec2Index) {
		projectile->vec2Index = vec2Index;
		projectileVector.push_back(projectile);
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

	//FOR SOUND - REAGAN
	size_t rreturn_vectorSize()
	{
		return enemyVector.size();
	}

	glm::vec2 return_enemyIndex(int i)
	{
		return enemyVector[i]->getVec2Index();
	}
};

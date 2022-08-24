#pragma once

//include dependencies
#include <iostream>

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

//include entities
#include "Map2D.h"
#include "Player2D.h"
#include "Enemy2D.h"
//enemy types
#include "Items.h"
//item types
#include "Projectile2D.h"
//projectile types

using namespace std;

class EventController : public CSingletonTemplate<EventController>
{
	friend CSingletonTemplate<EventController>;
private:
	CMap2D* cMap2D;
	CPlayer2D* cPlayer2D;
	//enemy
	//projectile
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
		//push to vector in scene2d
	}

	void spawnitems() {
		cout << "itemspawn" << endl;
		//push to vector in scene2d
	}

	void spawnprojectile() {
		cout << "projectilespawn" << endl;
		//push to vector in scene2d
	}

	void events() {			//controls events
		cout << "eventstarted" << endl;
	}


	void update() {			//runs every frame
		cout << "eventupdate" << endl;
	}
};

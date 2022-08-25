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
	CPlayer2D* cPlayer2D;
	//enemy
	vector<CEntity2D*> enemyVector;
	vector<CEntity2D*> projectileVector;
	//item
	//event

public:
	EventController() {

	}

	~EventController() {
		for (int i = 0; i < enemyVector.size(); i++)
		{
			delete enemyVector[i];
			enemyVector[i] = NULL;
		}
		enemyVector.clear();

		for (int i = 0; i < projectileVector.size(); i++)
		{
			delete projectileVector[i];
			projectileVector[i] = NULL;
		}
		projectileVector.clear();
	}

	void Init() {
		cPlayer2D = CPlayer2D::GetInstance();
		//creates all instances
		enemyVector.clear();
		projectileVector.clear();
		while (true)
		{
			Octopus* octo = new Octopus();
			octo->SetShader("Shader2D_Colour");

			if (octo->Init())
			{
				octo->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(octo);
			}
			else
				break;
		}
		while (true)
		{
			Chicken* chicken = new Chicken();
			chicken->SetShader("Shader2D_Colour");

			if (chicken->Init())
			{
				chicken->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(chicken);
			}
			else
				break;
		}
		/*while (true)
		{
			Spider* spider = new Spider();
			spider->SetShader("Shader2D_Colour");

			if (spider->Init())
			{
				spider->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(spider);
			}
			else
				break;
		}*/
	}

	void spawnEnemies() {
		
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

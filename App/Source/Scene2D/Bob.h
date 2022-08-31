#pragma once
#include "Enemy2D.h"
#include "BobShot.h"
#include <stdlib.h>

class Bob : public CEnemy2D
{
public:
	Bob();
	Bob(glm::vec2 pos, int hpMulti);
	~Bob();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
	bool randomDirection();
private:
	float shotInterval;
	float attackTimer;
	bool directionChosen;
};

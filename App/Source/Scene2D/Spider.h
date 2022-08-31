#pragma once
#include "Enemy2D.h"
#include "Webshot.h"
#include <stdlib.h>

class Spider : public CEnemy2D
{
public:
	Spider();
	Spider(glm::vec2 pos, int hpMulti);
	~Spider();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
	bool randomDirection();
	bool checkDirection(DIRECTION eDirection);
	glm::vec2 getVec2Index();
private:
	bool directionChosen;
	float shotInterval;
};

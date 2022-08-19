#pragma once
#include "Enemy2D.h"

class Chicken:public CEnemy2D
{
public:

	Chicken();
	~Chicken();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
private:
	bool stuck;
	bool collided;
	DIRECTION stuckDirection;
	glm::vec2 stuckPosition;
};

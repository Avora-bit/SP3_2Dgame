#pragma once
#include "Enemy2D.h"
#include <stdlib.h>

class Chicken:public CEnemy2D
{
public:
	Chicken();
	Chicken(glm::vec2 pos);
	~Chicken();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
	bool randomDirection();

	glm::vec2 getVec2Index();
private:

};

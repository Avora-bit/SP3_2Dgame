#pragma once
#include "Enemy2D.h"

class Octopus : public CEnemy2D
{
public:
	Octopus();
	Octopus(glm::vec2 pos, int hpMulti);
	~Octopus();

	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
	glm::vec2 getVec2Index();

private:
	bool stuck = false;
	DIRECTION stuckDirection;
	glm::vec2 stuckPosition;
};

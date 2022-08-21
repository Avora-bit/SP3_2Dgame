#pragma once
#include "Enemy2D.h"

class Octopus : protected CEnemy2D
{
public:
	Octopus();
	~Octopus();

	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);

private:
	bool stuck;
	bool collided;
	DIRECTION stuckDirection;
	glm::vec2 stuckPosition;
};

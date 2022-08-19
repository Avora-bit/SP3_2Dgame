#pragma once
#include "Enemy2D.h"

class Octopus : public CEnemy2D
{
public:
	Octopus();
	~Octopus();

	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);

private:
	bool stuck;
	DIRECTION stuckDirection;
	glm::vec2 stuckPosition;
};

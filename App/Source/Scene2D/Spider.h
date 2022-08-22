#pragma once
#include "Enemy2D.h"
#include <stdlib.h>

class Spider:public CEnemy2D
{
public:

	Spider();
	~Spider();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
	bool randomDirection();
	bool checkDirection(DIRECTION eDirection);
private:
	bool directionChosen;
};

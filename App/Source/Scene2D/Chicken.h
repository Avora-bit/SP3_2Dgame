#pragma once
#include "Enemy2D.h"
#include <stdlib.h>

class Chicken:public CEnemy2D
{
public:

	Chicken();
	~Chicken();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
	bool randomDirection();
private:

};

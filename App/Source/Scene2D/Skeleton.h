#pragma once
#include "Enemy2D.h"
#include "SkeletonShot.h"
#include <stdlib.h>

class Skeleton : public CEnemy2D
{
public:
	Skeleton();
	Skeleton(glm::vec2 pos);
	~Skeleton();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);
private:
	float shotInterval;
	float attackTimer;
};

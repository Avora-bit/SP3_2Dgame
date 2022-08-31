#pragma once
#include "Enemy2D.h"
#include "SkeletonShot.h"
#include <stdlib.h>

class Skeleton : public CEnemy2D
{
public:
	Skeleton();
	Skeleton(glm::vec2 pos, int hpMulti);
	~Skeleton();
	bool Init(void);
	void Update(const double dElapsedTime);
	void UpdatePosition(void);

	glm::vec2 getVec2Index();
private:
	float shotInterval;
	float attackTimer;
};

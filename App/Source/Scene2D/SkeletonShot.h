/**
 CShivs2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

#include "Projectile2D.h"

class SkeletonShot : public CProjectile2D
{

public:
	SkeletonShot();
	~SkeletonShot();
	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	bool InteractWithPlayer();
};


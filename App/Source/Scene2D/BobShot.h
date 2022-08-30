/**
 CShivs2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

#include "Projectile2D.h"

class BobShot : public CProjectile2D
{

public:
	BobShot();
	~BobShot();
	// Init
	bool Init(void);

	void setDirection(glm::vec2 direction);
	// Update
	void Update(const double dElapsedTime);

	bool InteractWithPlayer();
};


/**
 CShivs2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

#include "Projectile2D.h"

class Webshot : public CProjectile2D
{

public:
	Webshot();
	~Webshot();
	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	bool InteractWithPlayer();
};


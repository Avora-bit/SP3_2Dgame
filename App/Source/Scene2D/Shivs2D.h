/**
 CShivs2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

#include "Projectile2D.h"

#include "Enemy2D.h"

class CShivs2D : public CProjectile2D
{
public:
	CShivs2D();
	~CShivs2D();
	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	bool InteractWithEnemy(CEnemy2D* enemy);
};

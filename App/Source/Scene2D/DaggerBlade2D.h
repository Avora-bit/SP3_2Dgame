/**
CDaggerBlade2D
 */
#pragma once

// Include AnimatedSprites
#include "Blade2D.h"

class CDaggerBlade2D : public CBlade2D
{
public:
	// Constructor
	CDaggerBlade2D(void);

	// Destructor
	~CDaggerBlade2D(void);

	unsigned int LoadSprite();
};

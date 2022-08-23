/**
CRustyBlade2D
 */
#pragma once

// Include AnimatedSprites
#include "Blade2D.h"

class CRustyBlade2D : public CBlade2D
{
public:
	// Constructor
	CRustyBlade2D(void);

	// Destructor
	~CRustyBlade2D(void);

	unsigned int LoadSprite();
};

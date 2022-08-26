/**
CCleaverBlade2D
 */
#pragma once

// Include AnimatedSprites
#include "Blade2D.h"

class CCleaverBlade2D : public CBlade2D
{
public:
	// Constructor
	CCleaverBlade2D(void);

	// Destructor
	~CCleaverBlade2D(void);

	unsigned int LoadSprite();
};

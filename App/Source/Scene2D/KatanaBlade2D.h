/**
CKatanaBlade2D
 */
#pragma once

// Include AnimatedSprites
#include "Blade2D.h"

class CKatanaBlade2D : public CBlade2D
{
public:
	// Constructor
	CKatanaBlade2D(void);

	// Destructor
	~CKatanaBlade2D(void);

	unsigned int LoadSprite();
};

/**
CBlade2D
 */
#pragma once

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"
#include "InventoryItem.h"

class CBlade2D : public CInventoryItem
{
public:
	enum AILMENT {
		NONE = 0,
		POISON = 1,
		BURN = 2,
		BLEEDING = 3,
		AILMENT_NUM
	};
	// Constructor
	CBlade2D(void);

	// Destructor
	virtual ~CBlade2D(void);

	float getBaseDamage();
	float getBaseAtkSpd();
	float getBaseRange();
	float getBaseDef();

protected:
	float baseDamage;
	float baseAtkSpd;
	float baseRange;
	float baseDef;

	AILMENT effect;

	CSpriteAnimation* animatedSprites;
};

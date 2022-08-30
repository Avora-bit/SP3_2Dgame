/**
CBlade2D
 */
#pragma once

#include "Primitives/MeshBuilder.h"
// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

#include "InventoryItem.h"

class CBlade2D : public CInventoryItem
{
public:
	// Constructor
	CBlade2D(void);

	// Destructor
	virtual ~CBlade2D(void);

	float getBaseDamage();
	float getBaseAtkSpd();
	float getBaseRange();
	float getBaseDef();

	AILMENT getEffect();

	CSpriteAnimation* getAnimatedSprites();
	virtual unsigned int LoadSprite() = 0;
protected:
	float baseDamage;
	float baseAtkSpd;
	float baseRange;
	float baseDef;

	AILMENT effect;

	CSpriteAnimation* animatedSprites;
};

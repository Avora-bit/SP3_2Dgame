/**
 Blade2D
 */

#include "Blade2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

CBlade2D::CBlade2D(void)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CBlade2D::~CBlade2D(void)
{
	if (animatedSprites != nullptr)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}
}

float CBlade2D::getBaseDamage()
{
	return baseDamage;
}

float CBlade2D::getBaseAtkSpd()
{
	return baseAtkSpd;
}

float CBlade2D::getBaseRange()
{
	return baseRange;
}

float CBlade2D::getBaseDef()
{
	return baseDef;
}

CBlade2D::AILMENT CBlade2D::getEffect()
{
	return effect;
}

CSpriteAnimation* CBlade2D::getAnimatedSprites()
{
	return animatedSprites;
}

/**
 RustyBlade2D
 */

#include "RustyBlade2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

CRustyBlade2D::CRustyBlade2D(void)
{
	sName = "Rusty Blade";

	baseDamage = 10.0f;
	baseAtkSpd = 1.0f;
	baseRange = 2.0f;
	baseDef = 0.f;

	// animatedSprites =
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CRustyBlade2D::~CRustyBlade2D(void)
{
	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}
}
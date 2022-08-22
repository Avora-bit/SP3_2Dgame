﻿/**
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

	effect = CBlade2D::AILMENT::NONE;



	// animatedSprites =
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/woodenSword.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load sword tile texture" << std::endl;
	}
	else
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT);
		animatedSprites->AddAnimation("slash", 0, 0);
	}
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CRustyBlade2D::~CRustyBlade2D(void)
{
}
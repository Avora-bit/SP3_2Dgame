/**
 DaggerBlade2D
 */

#include "DaggerBlade2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

CDaggerBlade2D::CDaggerBlade2D(void)
{
	sName = "Dagger Blade";

	baseDamage = 8.0f;
	baseAtkSpd = 0.25f;
	baseRange = 1.5f;
	baseDef = 0.f;

	effect = CBlade2D::AILMENT::POISON;

	// animatedSprites =
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/daggerbladeslash.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load sword tile texture" << std::endl;
	}
	else
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 5, CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT);
		animatedSprites->AddAnimation("slash", 0, 4);
	}
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CDaggerBlade2D::~CDaggerBlade2D(void)
{
}

unsigned int CDaggerBlade2D::LoadSprite()
{
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/daggerbladeslash.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load sword tile texture" << std::endl;
		return 0;
	}
	else
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 5, CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT);
		animatedSprites->AddAnimation("idle", 0, 0);
		animatedSprites->AddAnimation("slash", 0, 4);

		return iTextureID;
	}
}
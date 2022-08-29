/**
 CleaverBlade2D
 */

#include "CleaverBlade2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

CCleaverBlade2D::CCleaverBlade2D(void)
{
	sName = "Cleaver Blade";

	baseDamage = 25.0f;
	baseAtkSpd = 1.4f;
	baseRange = 3.5f;
	baseDef = 0.f;

	effect = CBlade2D::AILMENT::BURN;

	// animatedSprites =
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/cleaverbladeslash.png", true);
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
CCleaverBlade2D::~CCleaverBlade2D(void)
{
}

unsigned int CCleaverBlade2D::LoadSprite()
{
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/cleaverbladeslash.png", true);
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
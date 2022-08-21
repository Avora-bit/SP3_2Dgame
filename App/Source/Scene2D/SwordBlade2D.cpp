/**
 SwordBlade2D
 */

#include "SwordBlade2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

CSwordBlade2D::CSwordBlade2D(void)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);


	baseDamage = 10.0f;
	baseAtkSpd = 1.0f;
	baseRange = 2.0f;
	baseDef = 0.f;

	effect = AILMENT::NONE;
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CSwordBlade2D::~CSwordBlade2D(void)
{
	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}
}
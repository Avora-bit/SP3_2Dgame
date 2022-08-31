/**
 Sword2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Sword2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Player2D.h"
#include "Primitives/MeshBuilder.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CSword2D::CSword2D(CHilt2D* hilt, CBlade2D* blade)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, animatedSprites(NULL)
	, cSoundController(NULL)
	, camera(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// Load the ground texture

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	this->hilt = hilt;
	this->blade = blade;
	guard = false;
	sName = "Sword";

	cPlayer2D = CPlayer2D::GetInstance();
	camera = Camera::GetInstance();
	cSettings = CSettings::GetInstance();

	SetShader("Shader2D_Colour");
	iTextureID = blade->LoadSprite();
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CSword2D::~CSword2D(void)
{
	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewherex
	cMap2D = NULL;

	camera = NULL;

	if (cSoundController)
	{
		cSoundController = NULL;
	}

	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}

	if (hilt)
	{
		delete hilt;
		hilt = NULL;
	}

	if (blade)
	{
		delete blade;
		blade = NULL;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

bool CSword2D::Init(void)
{
	return false;
}

/**
 @brief Update this instance
 */
void CSword2D::Update(const double dElapsedTime)
{
	//InteractWithMap();
	blade->getAnimatedSprites()->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = CSettings::GetInstance()->ConvertIndexToUVSpace(CSettings::GetInstance()->x, cPlayer2D->vec2Index.x, false, vec2NumMicroSteps.x * CSettings::GetInstance()->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = CSettings::GetInstance()->ConvertIndexToUVSpace(CSettings::GetInstance()->y, cPlayer2D->vec2Index.y, false, vec2NumMicroSteps.y * CSettings::GetInstance()->MICRO_STEP_YAXIS);
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CSword2D::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CSword2D::Render(void)
{
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtimeColour");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	transform = glm::scale(transform, glm::vec3(camera->zoom, camera->zoom, 0));

	transform = glm::translate(transform, 
		glm::vec3(vec2UVCoordinate.x + camera->vec2Index.x + cPlayer2D->vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS,
				vec2UVCoordinate.y + camera->vec2Index.y + cPlayer2D->vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS,
				0.0f));

	float angle = (atan2(camera->playerOffset.x, camera->playerOffset.y) / 3.14159265359) * 180.0;
	transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));

	vec2UVCoordinate.y = CSettings::GetInstance()->ConvertIndexToUVSpace(CSettings::GetInstance()->y, cPlayer2D->vec2Index.y - 1.f, false, (vec2NumMicroSteps.y - CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS/2)* CSettings::GetInstance()->MICRO_STEP_YAXIS);
	transform = glm::translate(transform,
		glm::vec3(vec2UVCoordinate.x + camera->pureVec2Index.x + cPlayer2D->vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS,
			vec2UVCoordinate.y + camera->pureVec2Index.y + (cPlayer2D->vec2NumMicroSteps.y - (getTotalRange()/2-1.0f) * cSettings->NUM_STEPS_PER_TILE_YAXIS) * cSettings->MICRO_STEP_YAXIS,
			0.0f));
	transform = glm::scale(transform, glm::vec3(-getTotalRange()*1.5, -getTotalRange(), 0));


	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(runtimeColour));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	blade->getAnimatedSprites()->Render();

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CSword2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

void CSword2D::InteractWithMap(void)
{
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x))
	{
	case 101:
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		vec2NumMicroSteps.x = 0;

		break;
	}
}

bool CSword2D::replaceHilt(CHilt2D* hilt)
{
	if (this->hilt->sName == hilt->sName)
		return false;
	this->hilt->~CHilt2D();
	this->hilt = hilt;
	return true;
}

bool CSword2D::replaceBlade(CBlade2D* blade)
{
	if (this->blade->sName == blade->sName)
		return false;
	this->blade->~CBlade2D();
	this->blade = blade;
	iTextureID = blade->LoadSprite();
	return true;
}

CHilt2D* CSword2D::getHilt()
{
	return hilt;
}

float CSword2D::getTotalDamage()
{
	return blade->getBaseDamage() + hilt->getSharp()*hilt->getBonusMultiplier()*5.0f;
}

float CSword2D::getTotalRavenous()
{
	return hilt->getRavenous() * hilt->getBonusMultiplier() * 5.0f;
}

float CSword2D::getTotalAtkSpeed()
{
	return blade->getBaseAtkSpd() * pow(0.85f, hilt->getLight()*hilt->getBonusMultiplier());
}

float CSword2D::getTotalRange()
{
	return blade->getBaseRange() + hilt->getWide()*hilt->getBonusMultiplier()*0.1f;
}

float CSword2D::getTotalDef()
{
	return blade->getBaseDef()+25.f*guard;
}

CBlade2D::AILMENT CSword2D::getEffect()
{
	return blade->getEffect();
}

CSpriteAnimation* CSword2D::getAnimatedSprites()
{
	return blade->getAnimatedSprites();
}
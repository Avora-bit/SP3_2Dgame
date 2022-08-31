/**
 CSword2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "..\SoundController\SoundController.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;
class CPlayer2D;
class Camera;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

#include "Physics2D.h"

#include "Camera.h"

#include "InventoryItem.h"
#include "GameManager.h"

#include "Hilt2D.h"
#include "Blade2D.h"

class CSword2D : public CInventoryItem
{
public:

	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Constructor
	CSword2D(CHilt2D* hilt, CBlade2D* blade);

	// Destructor
	virtual ~CSword2D(void);

	bool replaceHilt(CHilt2D* hilt);
	bool replaceBlade(CBlade2D* blade);
	CHilt2D* getHilt();

	float getTotalDamage();
	float getTotalRavenous();
	float getTotalAtkSpeed();
	float getTotalRange();
	float getTotalDef();
	CBlade2D::AILMENT getEffect();

	CSpriteAnimation* getAnimatedSprites();
protected:

	glm::vec2 vec2OldIndex;		//coords
	glm::vec2 direction;		//direction of movement

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	CPlayer2D* cPlayer2D;

	Camera* camera;

	//DIRECTION direction;
	// Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	CSoundController* cSoundController;

	CPhysics2D cPhysics2D;

	CHilt2D* hilt;
	CBlade2D* blade;
	bool guard;

	// Player's colour
	glm::vec4 runtimeColour;

	void InteractWithMap(void);
};


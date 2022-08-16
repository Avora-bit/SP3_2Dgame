/**
 CShivs2D
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

// Include CEntity2D
#include "Primitives/Entity2D.h"
#include "Primitives/SpriteAnimation.h"

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

#include "InventoryManager.h"
#include "GameManager.h"

class CShivs2D : public CSingletonTemplate<CShivs2D>, public CEntity2D
{
	friend CSingletonTemplate<CShivs2D>;
public:

	// Init
	bool Init(void);

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

protected:

	glm::vec2 vec2OldIndex;
	float rotation = 0.f;		//radian?

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	CPlayer2D* cPlayer2D;

	Camera* camera;

	double distanceTravelled;

	//DIRECTION direction;
	// Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	CSoundController* cSoundController;

	CPhysics2D cPhysics2D;

	// Player's colour
	glm::vec4 runtimeColour;

	// Constructor
	CShivs2D(void);

	// Destructor
	virtual ~CShivs2D(void);

	void InteractWithMap(void);

};


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
#include "Map2D.h"
#include "Player2D.h"
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

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include "Primitives/MeshBuilder.h"


#include <iostream>
using namespace std;

enum DIRECTION {
	UP = 0, DOWN, LEFT, RIGHT
};

class CProjectile2D : public CEntity2D
{
public:
	// Init
	virtual bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	void setDirection(glm::vec2 direction);

	bool bIsActive;
protected:

	int atk;
	int range;		//pending
	glm::vec2 vec2OldIndex;		//coords
	glm::vec2 vec2Direction;

	float mspeed;

	double distanceTravelled;

	float scaleX = 1.0f;
	float scaleY = 1.0f;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	CPlayer2D* cPlayer2D;

	Camera* camera;

	//DIRECTION direction;
	// Animated Sprite
	CSpriteAnimation* animatedSprites;

	CSoundController* cSoundController;

	CPhysics2D cPhysics2D;

	// Player's colour
	glm::vec4 runtimeColour;

	// Constructor
	CProjectile2D(void);

	// Destructor
	virtual ~CProjectile2D(void);

	// Constraint the player's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	void InteractWithMap(void);
	virtual bool InteractWithPlayer();

	bool CheckPosition(DIRECTION eDirection);

	void trajectory();
};


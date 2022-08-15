/**
 CPlayer2D
 @brief A class representing the player object
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

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;
class Camera;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

#include "..\SoundController\SoundController.h"
// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

#include "Physics2D.h"

#include "Camera.h"

#include "InventoryManager.h"
#include "GameManager.h"

class CPlayer2D : public CSingletonTemplate<CPlayer2D>, public CEntity2D
{
	friend CSingletonTemplate<CPlayer2D>;
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
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	glm::vec2 vec2OldIndex;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	//CS: The quadMesh for drawing the tiles
	CMesh* quadMesh;

	CPhysics2D cPhysics2D;

	CInventoryManager* cInventoryManager;

	CInventoryItem* cInventoryItem;

	CSoundController* cSoundController;

	Camera* camera;

	DIRECTION direction;
	DIRECTION attackDirection;

	bool chargeSword;
	bool hasSword;
	bool slash;
	double swordForce;

	// Player's colour
	glm::vec4 runtimeColour;

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Constraint the player's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	void InteractWithMap(void);

	bool CheckPosition(DIRECTION eDirection);

	void UpdateHealthLives(void);

public:
	bool getHasSword(void);
	void SetHasSword(bool hasSword);

	bool getSlash(void);
	void setSlash(bool slash);

	DIRECTION getAttackDirection();
	bool getChargeSword();
	double getSwordForce();

	void LoseHealth(float health);


	float speed_multiplier = 0.25f;
};


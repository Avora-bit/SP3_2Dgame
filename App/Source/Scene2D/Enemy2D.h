/**
 CEnemy2D
 @brief A class which represents the enemy object
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include shader
#include "RenderControl\shader.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Settings
#include "GameControl\Settings.h"

// Include Physics2D
#include "Physics2D.h"

// Include Player2D
#include "Player2D.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include Sprite Animations
#include "Primitives/SpriteAnimation.h"

#include "Camera.h"
#include "Map2D.h"

#include <iostream>
using namespace std;


class CEnemy2D : public CEntity2D
{
public:
	// Constructor
	CEnemy2D(void);

	// Destructor
	virtual ~CEnemy2D(void);

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

	// Set the handle to cPlayer to this class instance
	void SetPlayer2D(CPlayer2D* cPlayer2D);

	// boolean flag to indicate if this enemy is active
	bool bIsActive;

	glm::vec2 getVec2Index();
	glm::vec2 getVec2MicroSteps();

	void takeDamage(float damage);

	void SetStatus(AILMENT status, float time);

	float getHealth();
	float getMaxHealth();
	bool sleep;

	float getScale();
	glm::vec2 getPreciseVec2Index(bool toOrigin);
protected:
	enum DIRECTION
	{
		LEFT = 0,
		TOPLEFT,
		UP,
		TOPRIGHT,
		RIGHT,
		BOTTOMRIGHT,
		DOWN,
		BOTTOMLEFT,
		NUM_DIRECTIONS
	};

	enum FSM
	{
		IDLE = 0,
		PATROL,
		CHASE,
		ATTACK,
		RUN,
		SLEEP,
		BOSSPHASE1,
		BOSSPHASE2,
		NUM_FSM
	};

	AILMENT status;
	float statusTimer;

	glm::vec2 vec2OldIndex;
	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	float angle;

	// The vec2 variable which stores The number of microsteps from the tile indices for the enemy2D. 
	// A tile's width or height is in multiples of these microsteps
	glm::vec2 vec2NumMicroSteps;

	// The vec2 variable which stores the UV coordinates to render the enemy2D
	glm::vec2 vec2UVCoordinate;

	// The vec2 which stores the indices of the destination for enemy2D in the Map2D
	glm::vec2 vec2Destination;
	// The vec2 which stores the direction for enemy2D movement in the Map2D
	glm::vec2 vec2Direction;

	// Settings
	CSettings* cSettings;

	// Physics
	CPhysics2D cPhysics2D;

	// Current color
	glm::vec4 runtimeColour;

	CSpriteAnimation* animatedSprites;

	// Handle to the CPlayer2D
	CPlayer2D* cPlayer2D;

	Camera* camera;

	CSoundController* cSoundController;

	// Current FSM
	FSM sCurrentFSM;

	// FSM counter - count how many frames it has been in this FSM
	int iFSMCounter;

	float health;
	float maxHealth;

	float timer;
	// Max count in a state
	const int iMaxFSMCounter = 60;

	// Constraint the enemy2D's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	// Check if a position is possible to move into
	bool CheckPosition(DIRECTION eDirection);

	// Let enemy2D interact with the player
	bool InteractWithPlayer(void);

	// Update direction
	void UpdateDirection(void);

	// Update position
	void UpdatePosition(void);

	float speed_multiplier = 1.00f;
	void UpdateStatus(const double dElapsedTime);

	float atk;
	float scaleX = 1;
	float scaleY = 1;
};


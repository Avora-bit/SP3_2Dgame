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

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;
class Camera;

#include "../GameStateManagement/GameStateBase.h"

// Include Keyboard controller
#include "Inputs\KeyboardController.h"
#include "Inputs\MouseController.h"

#include "..\SoundController\SoundController.h"
// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

#include "Physics2D.h"

#include "Camera.h"

#include "InventoryManager.h"
#include "GameManager.h"

#include "slot.h"



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

	bool getAttacking();

	void setActive(int arr, bool act);
	bool getActive(int arr);

protected:
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		TOP_LEFT = 4,
		BOTTOM_LEFT = 5,
		TOP_RIGHT = 6,
		BOTTOM_RIGHT = 7,
		NUM_DIRECTIONS
	};

	glm::vec2 vec2OldIndex;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// Animated Sprite
	CSpriteAnimation* animatedSprites;

	CKeyboardController* cKeyboardController;
	CMouseController* cMouseController;

	CGameStateBase* cGameStateBase;
	CPhysics2D cPhysics2D;

	CInventoryManager* cInventoryManager;
	CInventoryItem* cInventoryItem;

	CSoundController* cSoundController;

	CGameManager* cGameManager;

	ISound* dodgesfx;
	ISound* grasssfx;
	ISound* watersfx;
	ISound* sandsfx;
	ISound* firesfx;
	ISound* enemysfx;
	float walkingTime;

	float cooking_time;
	bool cooking_mode;

	glm::vec2 campfireVec2;

	float soundVol;

	Camera* camera;

	DIRECTION direction;
	DIRECTION attackDirection;

	bool throwing;
	double maxPForce;
	double minPForce;
	double ProjectileForce;

	// vitals
	double invincibility; 

	bool dashTrue;

	float movementSpeed; 

	bool attacking;
	float attackTimer;

	//bool shovelcheck = true;

	//render
	float angle;

	// Player's colour
	glm::vec4 runtimeColour;

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Constraint the player's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);


	

	bool reset_pos();
	void InteractWithMap(void);

	void AttackEnemy();

	bool CheckPosition(DIRECTION eDirection);

	void UpdateHealthLives(void);

	slot inventorySlots[9];
	CImageLoader* il;


	map<int, int> MapOfTextureIDs;

	int octopusKillCount;
	int spiderKillCount;
	int chickenKillCount;
	int skeletonKillCount;

public:
	DIRECTION getAttackDirection();
	double getProjectileForce();

	void LoseHealth(float health);

	bool AddItem(int itemid);
	slot getitem(int arr);
	void setitem(int arr, int itemid);

	void setitemquantity(int arr, int quantity);

	int getitemval(int arr);
	int gettextureid(int arr);

	int getOctopusKillCount();
	int getSpiderKillCount();
	int getChickenKillCount();
	int getSkeletonKillCount();

	void addOctopusKillCount(int kill);
	void addSpiderKillCount(int kill);
	void addChickenKillCount(int kill);
	void addSkeletonKillCount(int kill);


	void setsound(float vol);
	float returnsound();
};
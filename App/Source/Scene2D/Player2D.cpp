/**
 Player2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"

#include "Enemy2D.h"

#include "Sword2D.h"
#include "WoodenHilt2D.h"
#include "IronHilt2D.h"
#include "PlatinumHilt2D.h"
#include "RustyBlade2D.h"
#include "CleaverBlade2D.h"
#include "DaggerBlade2D.h"
#include "KatanaBlade2D.h"
#include "Shivs2D.h"


#include "../GameStateManagement/CraftingState.h"

#include "EventController.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cMouseController(NULL)
	, cKeyboardController(NULL)
	, cGameManager(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, animatedSprites(NULL)
	, cSoundController(NULL)
	, camera(NULL)
	, dodgesfx(NULL)
	, watersfx(NULL)
	, grasssfx(NULL)
	, sandsfx(NULL)
	, firesfx(NULL)
	,enemysfx(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	cKeyboardController = NULL;
	cMouseController = NULL;

	cMap2D = NULL;

	camera = NULL;

	if (cGameManager)
	{
		cGameManager = NULL;
	}

	if (cSoundController)
	{
		cSoundController = NULL;
	}

	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}

	
	if (dodgesfx)
	{
		delete dodgesfx;
		dodgesfx = nullptr;
	}
	if (grasssfx)
	{
		delete grasssfx;
		grasssfx = nullptr;
	}
	if (watersfx)
	{
		delete watersfx;
		watersfx = nullptr;
	}
	if (sandsfx)
	{
		delete sandsfx;
		sandsfx = nullptr;
	}
	if (firesfx)
	{
		delete firesfx;
		firesfx = nullptr;
	}
	if (enemysfx)
	{
		delete enemysfx;
		enemysfx = nullptr;
	}
	
	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	cKeyboardController = CKeyboardController::GetInstance();
	cMouseController = CMouseController::GetInstance();

	camera = Camera::GetInstance();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	cGameManager = CGameManager::GetInstance();

	cInventoryManager = CInventoryManager::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = 1;
	unsigned int uiCol = 1;
	if (cMap2D->FindValue(200, uiRow, uiCol, true, 1)) {
		cMap2D->SetMapInfo(uiRow, uiCol, 0, true, 1);
	}
	else if (cMap2D->FindValue(78, uiRow, uiCol, true, 1)) {}
	else if (cMap2D->FindValue(77, uiRow, uiCol, true, 1)) {}
	else return false;		//unable to find valid position to spawn player


	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	ProjectileForce = 0;

	walkingTime = 0;

	angle = 0;

	direction = RIGHT;

	soundVol = 1.f;

	throwing = false;
	maxPForce = 10;
	minPForce = 5;
	ProjectileForce = 0;

	// vitals
	invincibility = 0;

	dashTrue = true;

	movementSpeed = 1.f;
	attacking = false;
	attackTimer = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load the player texture
	// Load the ground texture

	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/mc.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 0);
	animatedSprites->AddAnimation("walk", 0, 2);
	animatedSprites->PlayAnimation("idle", -1, 0.3f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	cInventoryManager = CInventoryManager::GetInstance();
	/*
	cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	*/
	// vitals
	cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Stamina", "Image/stamina.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Hunger", "Image/hunger_logo.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	

	//Add ITEMS
	cInventoryItem = cInventoryManager->Add("Stick", "Image/Sp3Images/Base/stick.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	cInventoryItem = cInventoryManager->Add("Wood", "Image/Sp3Images/Base/wood.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	cInventoryItem = cInventoryManager->Add("Shivs", "Image/Sp3Images/Weapons/shiv.png", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Rock", "Image/Sp3Images/Base/rock.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Swords", "Image/Sp3Images/Weapons/sword.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Campfire", "Image/Campfire.tga", 0, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);


	cInventoryItem = cInventoryManager->Add("Raw Food", "Image/Sp3Images/Food/Raw_food.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Cooked Food", "Image/Sp3Images/Food/Cooked_food.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Shovel", "Image/Sp3Images/Weapons/shovel.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);


	cInventoryItem = cInventoryManager->Add("pHilt", "Image/Sp3Images/Weapons/Hilts/ironHilt.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("wHilt", "Image/Sp3Images/Weapons/Hilts/platinumHilt.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("iHilt", "Image/Sp3Images/Weapons/Hilts/woodenHilt.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);


	cInventoryItem = cInventoryManager->Add("cBlade", "Image/Sp3Images/Weapons/Blades/cleaverblade.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("kBlade", "Image/Sp3Images/Weapons/Blades/katanablade.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("rBlade", "Image/Sp3Images/Weapons/Blades/placeholder_RustyBlade.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("dBlade", "Image/Sp3Images/Weapons/Blades/daggerblade.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	//sword->replaceBlade(new CDaggerBlade2D());

	cInventoryItem->vec2Size = glm::vec2(25, 25);
	cSoundController = CSoundController::GetInstance();

	il = CImageLoader::GetInstance();
	
	cooking_mode = false;
	cooking_time = 5.f;
	campfireVec2.x = 0;
	campfireVec2.y = 0;

	//set inventory slots to 0 at the start of the game
	for (int i = 0; i < 9; i++) {
		inventorySlots[i].setitemID(0);
	}

	//CSword2D* sword = new CSword2D(new CPlatinumHilt2D(), new CDaggerBlade2D());
	//CInventoryManager::GetInstance()->Add(sword);

	octopusKillCount = 0;
	chickenKillCount = 0;
	spiderKillCount = 0;
	skeletonKillCount = 0;
	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
	
	return true;
}

/**
 @brief Update this instance
 */
void CPlayer2D::Update(const double dElapsedTime)
{
	// add attributes when kill count is met
	if (octopusKillCount >= 5)
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->getHilt()->addWide(1);
		octopusKillCount = 0;
	}
	if (chickenKillCount >= 5)
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->getHilt()->addLight(1);
		chickenKillCount = 0;
	}
	if (spiderKillCount >= 3)
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->getHilt()->addRavenous(1);
		spiderKillCount = 0;
	}
	if (skeletonKillCount >= 1)
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->getHilt()->addSharp(1);
		skeletonKillCount = 0;
	}
	//invincibility timer
	{
		if (invincibility > 0) {
			invincibility -= dElapsedTime;
		}
		else if (invincibility < 0) {
			invincibility = 0;
		}
		else {
			//null
		}
	}

	//PLAY SOUND DEPENDING ON SURFACE TYPE
	if (cKeyboardController->IsKeyDown(GLFW_KEY_A)
		|| cKeyboardController->IsKeyDown(GLFW_KEY_S)
		|| cKeyboardController->IsKeyDown(GLFW_KEY_D)
		|| cKeyboardController->IsKeyDown(GLFW_KEY_W))
	{
		walkingTime += 0.5f * dElapsedTime;
		if (walkingTime >= 1)
		{
			walkingTime = 1;
		}
		switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 0))
		{
		case 99: //grass
		{
			//walkingTime = 0;

			if (sandsfx != nullptr)
			{
				sandsfx->setVolume(0.f);
				//sandsfx = nullptr;
			}
			if (watersfx != nullptr)
			{
				watersfx->setVolume(0.f);
				//watersfx = nullptr;
			}

			ISound* grassSound = cSoundController->PlaySoundByID_2(7);
			if (grassSound != nullptr)
			{
				grasssfx = grassSound;
			}
			if (grasssfx != nullptr)
			{
				grasssfx->setVolume(soundVol * walkingTime);
			}

			break;
		}
		case 98: //sand
		{
			//walkingTime = 0;

			if (grasssfx != nullptr)
			{
				grasssfx->setVolume(0.f);
				//grasssfx = nullptr;
			}

			if (watersfx != nullptr)
			{
				watersfx->setVolume(0.f);
				//watersfx = nullptr;
			}

			ISound* sandSound = cSoundController->PlaySoundByID_2(9);
			if (sandSound != nullptr)
			{
				sandsfx = sandSound;
			}
			if (sandsfx != nullptr)
			{
				sandsfx->setVolume(soundVol * walkingTime);
			}
			break;
		}
		case 97: //water
		{
			//walkingTime = 0;

			if (grasssfx != nullptr)
			{
				grasssfx->setVolume(0.f);
				//grasssfx = nullptr;
			}
			if (sandsfx != nullptr)
			{
				sandsfx->setVolume(0.f);
				//sandsfx = nullptr;
			}

			ISound* waterSound = cSoundController->PlaySoundByID_2(8);
			if (waterSound != nullptr)
			{
				watersfx = waterSound;
			}
			if (watersfx != nullptr)
			{
				watersfx->setVolume(soundVol* walkingTime);

			}
			break;
		}
		default:
		{
			break;
		}
		}

	}
	else
	{
		walkingTime = 0;

		if (grasssfx != nullptr)
		{
			grasssfx->setVolume(0.f);
			//grasssfx = nullptr;
		}
		if (sandsfx != nullptr)
		{
			sandsfx->setVolume(0.f);
			//sandsfx = nullptr;
		}
		if (watersfx != nullptr)
		{
			watersfx->setVolume(0.f);
			//watersfx = nullptr;
		}

	}
	vec2OldIndex = vec2Index;

	// vitals
	static float hungerTimer = 0;
	hungerTimer += dElapsedTime;


	//FORAGE FOR TREES(Get sticks)
	if (cMouseController->IsButtonDown(0))
	{
		//right
		if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1, true, 1) == 100 /*&& direction == 1*/)
		{
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x + 1, 30, true, 1);
		}
		//left
		if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - 1, true, 1) == 100
			/*&& direction == 0*/)
		{
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x - 1, 30, true, 1);
		}
		//down
		if (vec2Index.y != 0 && cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x, true, 1) == 100
			/*&& direction == 3*/)
		{
			cMap2D->SetMapInfo(vec2Index.y - 1, vec2Index.x, 30, true, 1);
		}
		//up
		if (vec2Index.y != cSettings->NUM_TILES_YAXIS - 1 && cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x, true, 1) == 100
			/*&& direction == 2*/)
		{
			cMap2D->SetMapInfo(vec2Index.y + 1, vec2Index.x, 30, true, 1);
		}
	}

	if (cKeyboardController->IsKeyReleased(GLFW_KEY_1))
	{
		//PLACE CAMPFIRE
		if (inventorySlots[0].getitemID() == 102)
		{
			cMap2D->SetMapInfo(vec2Index.y + 1, vec2Index.x, 102, true, 1);
			inventorySlots[0].setquantity(0);
		}
		//COOK FOOD
		else if (inventorySlots[0].getitemID() == 70)
		{
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1, 102)
				/*&& direction == 1*/)
				|| (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - 1, 102)
					/*	&& direction == 0*/)
				|| (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x, 102)
					/*	&& direction == 3*/)
				|| (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x, 102)
					/*	&& direction == 2*/))
			{
				inventorySlots[0].SubtractQuantity(1);

				cooking_mode = true;
				campfireVec2.y = vec2Index.y;
				campfireVec2.x = vec2Index.x;
			}
		}
		//EAT FOOD
		else if (inventorySlots[0].getitemID() == 81)
		{
			cInventoryItem = cInventoryManager->GetItem("Health");
			cInventoryItem->Add(30);
			cInventoryItem = cInventoryManager->GetItem("Hunger");
			cInventoryItem->Add(80);
			inventorySlots[0].SubtractQuantity(1);
		}
	}
	if (cKeyboardController->IsKeyReleased(GLFW_KEY_2))
	{
		//PLACE CAMPFIRE
		if (inventorySlots[1].getitemID() == 102)
		{
			cMap2D->SetMapInfo(vec2Index.y + 1, vec2Index.x, 102, true, 1);
			inventorySlots[1].setquantity(0);

		}
		//COOK FOOD
		else if (inventorySlots[1].getitemID() == 70)
		{

			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1, 102)
				/*&& direction == 1*/)
				|| (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - 1, 102)
					/*	&& direction == 0*/)
				|| (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x, 102)
					/*	&& direction == 3*/)
				|| (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x, 102)
					/*	&& direction == 2*/))
			{
				inventorySlots[1].SubtractQuantity(1);

				cooking_mode = true;
				campfireVec2.y = vec2Index.y;
				campfireVec2.x = vec2Index.x;
			}
		}
		//EAT FOOD
		else if (inventorySlots[1].getitemID() == 81)
		{
			cInventoryItem = cInventoryManager->GetItem("Health");
			cInventoryItem->Add(30);
			cInventoryItem = cInventoryManager->GetItem("Hunger");
			cInventoryItem->Add(80);
			inventorySlots[1].SubtractQuantity(1);

		}
	}
	if (cKeyboardController->IsKeyReleased(GLFW_KEY_3))
	{
		//PLACE CAMPFIRE
		if (inventorySlots[2].getitemID() == 102)
		{
			cMap2D->SetMapInfo(vec2Index.y + 1, vec2Index.x, 102, true, 1);
			inventorySlots[2].setquantity(0);

		}
		//COOK FOOD
		else if (inventorySlots[2].getitemID() == 70)
		{
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1, 102)
				/*&& direction == 1*/)
				|| (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - 1, 102)
					/*	&& direction == 0*/)
				|| (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x, 102)
					/*	&& direction == 3*/)
				|| (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x, 102)
					/*	&& direction == 2*/))
			{
				inventorySlots[2].SubtractQuantity(1);

				cooking_mode = true;
				campfireVec2.y = vec2Index.y;
				campfireVec2.x = vec2Index.x;
			}
		}
		//EAT FOOD
		else if (inventorySlots[2].getitemID() == 81)
		{
			cInventoryItem = cInventoryManager->GetItem("Health");
			cInventoryItem->Add(30);
			cInventoryItem = cInventoryManager->GetItem("Hunger");
			cInventoryItem->Add(80);
			inventorySlots[2].SubtractQuantity(1);

		}
	}

	//TIMER TO COOK FOOD
	if (cooking_mode)
	{
		cooking_time -= 1.f * dElapsedTime;
	}

	if (cooking_time <= 0)
	{
		cooking_mode = false;
		cMap2D->SetMapInfo(vec2Index.y + 2, vec2Index.x, 81, true, 1);
		cooking_time = 5.f;
	}

	//SET INVENTORY TEXTURE ID To 0 IF QUANTITY IS 0
	for (int i = 0; i < 9; i++)
	{
		if (inventorySlots[i].getquantity() == 0)
		{
			inventorySlots[i].setitemID(0);
			inventorySlots[i].settextureID(0);
		}
	}

	if (hungerTimer >= 10 && cInventoryManager->GetItem("Hunger")->GetCount() > 0)
	{
		cInventoryManager->GetItem("Hunger")->Remove(5);
		hungerTimer = 0;
	}

	static float starveTimer = 0;
	if (cInventoryManager->GetItem("Hunger")->GetCount() <= 0)
	{
		starveTimer += dElapsedTime;
		if (starveTimer >= 1)
		{
			starveTimer = 0;
			cInventoryManager->GetItem("Health")->Remove(5);
		}
	}
	else if (cInventoryManager->GetItem("Hunger")->GetCount() > 0 && starveTimer > 0)
		starveTimer = 0;


	if (cInventoryManager->GetItem("Health")->GetCount() <= 0)
		CGameManager::GetInstance()->bPlayerLost = true;

	static bool walkKeyDown = false;
	if ((cKeyboardController->IsKeyDown(GLFW_KEY_W) || cKeyboardController->IsKeyDown(GLFW_KEY_A) || cKeyboardController->IsKeyDown(GLFW_KEY_S) || cKeyboardController->IsKeyDown(GLFW_KEY_D)) && !walkKeyDown)
	{
		walkKeyDown = true;
		animatedSprites->PlayAnimation("walk", -1, 0.5f);
	}
	else if ((!cKeyboardController->IsKeyDown(GLFW_KEY_W) && !cKeyboardController->IsKeyDown(GLFW_KEY_A) && !cKeyboardController->IsKeyDown(GLFW_KEY_S) && !cKeyboardController->IsKeyDown(GLFW_KEY_D)) && walkKeyDown)
	{
		walkKeyDown = false;
		animatedSprites->PlayAnimation("idle", -1, 1.f);
	}

	static bool leftClickDown = false;
	if (cInventoryManager->Check("Sword"))
	{
		static float attackTimer = 0;
		attackTimer += dElapsedTime;
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		if (attackTimer > sword->getTotalAtkSpeed())
		{
			attacking = false;
			sword->getAnimatedSprites()->PlayAnimation("idle", -1, 0.1f);
		}
		if (cMouseController->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT) && !leftClickDown && !attacking)
		{
			attacking = true;
			leftClickDown = true;

			sword->getAnimatedSprites()->PlayAnimation("slash", 0, sword->getTotalAtkSpeed());
			attackTimer = 0;

			AttackEnemy();
		}
		else if (!cMouseController->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT) && leftClickDown && !attacking)
		{
			leftClickDown = false;
		}
	}
	
	
	static float staminaTimer = 0;
	if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::DODGE)
	{
		staminaTimer += dElapsedTime;
		if (cInventoryManager->GetItem("Stamina")->GetCount() < cInventoryManager->GetItem("Stamina")->GetMaxCount()) // stamina
		{
			if (staminaTimer >= 0.05)
			{
				cInventoryManager->GetItem("Stamina")->Add(1);
				staminaTimer = 0;
			}
		}
		if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
		{
			if (vec2Index.x >= 0)
			{
				vec2NumMicroSteps.x -= movementSpeed;
				if (vec2NumMicroSteps.x < 0)
				{
					vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
					vec2Index.x--;
				}
			}

			if (!CheckPosition(LEFT))
			{
				vec2Index.x = vec2OldIndex.x;
				vec2NumMicroSteps.x = 0;
			}
			Constraint(LEFT);

			/*if (hasSword || chargeSword)
				animatedSprites->PlayAnimation("walkLeftSW", -1, 0.1f);
			else
				animatedSprites->PlayAnimation("walkLeft", -1, 0.15f);*/

			//angle = 270;
			direction = LEFT;
		}

		if (cKeyboardController->IsKeyDown(GLFW_KEY_S)) {
			if (vec2Index.y >= 0)
			{
				vec2NumMicroSteps.y -= movementSpeed;
				if (vec2NumMicroSteps.y < 0)
				{
					vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
					vec2Index.y--;
				}
			}

			Constraint(DOWN);
			if (!CheckPosition(DOWN))
			{
				vec2Index.y = vec2OldIndex.y;
				vec2NumMicroSteps.y = 0;
			}
			direction = DOWN;
		}
		if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
		{
			if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
			{
				vec2NumMicroSteps.y += movementSpeed;
				if (vec2NumMicroSteps.y >= cSettings->NUM_STEPS_PER_TILE_YAXIS)
				{
					vec2NumMicroSteps.y = 0;
					vec2Index.y++;
				}
			}

			if (!CheckPosition(UP))
			{
				vec2NumMicroSteps.y = 0;
			}
			Constraint(UP);

			/*if (hasSword || chargeSword)
				animatedSprites->PlayAnimation("walkRightSW", -1, 0.1f);
			else
				animatedSprites->PlayAnimation("walkRight", -1, 0.15f);*/

			//angle = 180;
			direction = UP;
		}
		if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
		{
			if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
			{
				vec2NumMicroSteps.x += movementSpeed;
				if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
				{
					vec2NumMicroSteps.x = 0;
					vec2Index.x++;
				}
			}

			if (!CheckPosition(RIGHT))
			{
				vec2NumMicroSteps.x = 0;
			}
			Constraint(RIGHT);

			/*if (hasSword || chargeSword)
				animatedSprites->PlayAnimation("walkRightSW", -1, 0.1f);
			else
				animatedSprites->PlayAnimation("walkRight", -1, 0.15f);*/
			//angle = 90;
			direction = RIGHT;
		}

		if (cKeyboardController->IsKeyDown(GLFW_KEY_W) && cKeyboardController->IsKeyDown(GLFW_KEY_D)) // top right
		{
			direction = TOP_RIGHT;
			//angle = 135;
		}
		else if (cKeyboardController->IsKeyDown(GLFW_KEY_S) && cKeyboardController->IsKeyDown(GLFW_KEY_D)) // bottom right
		{
			direction = BOTTOM_RIGHT;
			//angle = 45;
		}
		else if (cKeyboardController->IsKeyDown(GLFW_KEY_S) && cKeyboardController->IsKeyDown(GLFW_KEY_A)) // bottom left
		{
			direction = BOTTOM_LEFT;
			//angle = 315;
		}
		else if (cKeyboardController->IsKeyDown(GLFW_KEY_W) && cKeyboardController->IsKeyDown(GLFW_KEY_A)) // top left
		{
			direction = TOP_LEFT;
			//angle = 225;
		}

		static bool dodgeKeyDown = false;
		if ((cKeyboardController->IsKeyDown(GLFW_KEY_SPACE) || cKeyboardController->IsKeyDown(GLFW_KEY_LEFT_SHIFT)) && !dodgeKeyDown &&
			cInventoryManager->GetItem("Stamina")->GetCount() >= 30.f && dashTrue)
		{
			dodgeKeyDown = true;
			cInventoryManager->GetItem("Stamina")->Remove(30.f);
			cPhysics2D.SetStatus(CPhysics2D::STATUS::DODGE);
			cPhysics2D.SetInitialVelocity(glm::vec2(2.5f, 0.0f));

			//Sound to make dodge
			ISound* dodgeSound = cSoundController->PlaySoundByID_2(5);
			if (dodgeSound != nullptr)
			{
				dodgesfx = dodgeSound;
			}
			if (dodgesfx != nullptr)
			{
				dodgesfx->setVolume(soundVol * .8f);
			}
			
		}
		else if (!cKeyboardController->IsKeyDown(GLFW_KEY_SPACE) && !cKeyboardController->IsKeyDown(GLFW_KEY_LEFT_SHIFT) && dodgeKeyDown)
			dodgeKeyDown = false;
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::DODGE)
	{
		static float dodgeTimer = 0;
		dodgeTimer += dElapsedTime;
		if (staminaTimer > 0)
			staminaTimer = 0;
		
		cPhysics2D.SetAcceleration(glm::vec2(-8.0f, 0.0f));
		cPhysics2D.SetTime(float(dElapsedTime));
		cPhysics2D.Update();
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();
		glm::vec2 iIndex_OLD = vec2Index;
		int iDisplacement_MicroSteps = (int)(v2Displacement.x / 0.015625f);
		switch (direction)
		{
		case UP:
		{
			if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
			{
				vec2NumMicroSteps.y += fabs(iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.y > (int)cSettings->NUM_STEPS_PER_TILE_YAXIS)
				{
					vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
					if (vec2NumMicroSteps.y < 0)
						vec2NumMicroSteps.y = 0;
					vec2Index.y++;
				}
			}
			Constraint(UP);
			int iIndex_YAxis_Proposed = vec2Index.y;
			for (int i = iIndex_OLD.y; i <= iIndex_YAxis_Proposed; i++)
			{
				vec2Index.y = i;
				if (CheckPosition(UP) == false)
				{
					vec2NumMicroSteps.y = 0;
					break;
				}
			}
			break;
		}
		case DOWN:
		{
			if (vec2Index.y >= 0)
			{
				vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.y <= 0)
				{
					vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
					vec2Index.y--;
				}
			}
			Constraint(DOWN);
			int iIndex_YAxis_Proposed = vec2Index.y;
			for (int i = iIndex_OLD.y; i >= iIndex_YAxis_Proposed; i--)
			{
				vec2Index.y = i;
				if (CheckPosition(DOWN) == false)
				{
					vec2Index.y = vec2OldIndex.y;
					vec2NumMicroSteps.y = 0;
					break;
				}
			}
			break;
		}
		case LEFT:
		{
			if (vec2Index.x >= 0)
			{
				vec2NumMicroSteps.x -= fabs(iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.x <= 0)
				{
					vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
					vec2Index.x--;
				}
			}
			Constraint(LEFT);
			int iIndex_XAxis_Proposed = vec2Index.x;
			for (int i = iIndex_OLD.x; i >= iIndex_XAxis_Proposed; i--)
			{
				vec2Index.x = i;
				if (CheckPosition(LEFT) == false)
				{
					vec2Index.x = vec2OldIndex.x;
					vec2NumMicroSteps.x = 0;
					break;
				}
			}
			break;
		}
		case RIGHT:
		{
			if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
			{
				vec2NumMicroSteps.x += iDisplacement_MicroSteps;
				if (vec2NumMicroSteps.x > cSettings->NUM_STEPS_PER_TILE_XAXIS)
				{
					vec2NumMicroSteps.x -= cSettings->NUM_STEPS_PER_TILE_XAXIS;
					if (vec2NumMicroSteps.x < 0)
						vec2NumMicroSteps.x = 0;
					vec2Index.x++;
				}
			}
			Constraint(RIGHT);
			int iIndex_XAxis_Proposed = vec2Index.x;
			for (int i = iIndex_OLD.x; i <= iIndex_XAxis_Proposed; i++)
			{
				vec2Index.x = i;
				if (CheckPosition(RIGHT) == false)
				{
					vec2NumMicroSteps.x = 0;
					break;
				}
			}
			break;
		}
		case TOP_LEFT:
		{
			if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
			{
				vec2NumMicroSteps.y += fabs(sin(glm::radians(45.f)) * iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.y > (int)cSettings->NUM_STEPS_PER_TILE_YAXIS)
				{
					vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
					if (vec2NumMicroSteps.y < 0)
						vec2NumMicroSteps.y = 0;
					vec2Index.y++;
				}
			}
			Constraint(UP);
			int iIndex_YAxis_Proposed = vec2Index.y;
			for (int i = iIndex_OLD.y; i >= iIndex_YAxis_Proposed; i--)
			{
				vec2Index.y = i;
				if (CheckPosition(UP) == false)
				{
					vec2Index.y = vec2OldIndex.y;
					vec2NumMicroSteps.y = 0;
					break;
				}
			}

			if (vec2Index.x >= 0)
			{
				vec2NumMicroSteps.x -= fabs(cos(glm::radians(45.f)) * iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.x <= 0)
				{
					vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
					vec2Index.x--;
				}
			}
			Constraint(LEFT);
			int iIndex_XAxis_Proposed = vec2Index.x;
			for (int i = iIndex_OLD.x; i >= iIndex_XAxis_Proposed; i--)
			{
				vec2Index.x = i;
				if (CheckPosition(LEFT) == false)
				{
					vec2Index.x = vec2OldIndex.x;
					vec2NumMicroSteps.x = 0;
					break;
				}
			}
			break;
		}
		case BOTTOM_LEFT:
		{
			if (vec2Index.y >= 0)
			{
				vec2NumMicroSteps.y -= fabs(sin(glm::radians(45.f)) * iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.y <= 0)
				{
					vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
					vec2Index.y--;
				}
			}
			Constraint(DOWN);
			int iIndex_YAxis_Proposed = vec2Index.y;
			for (int i = iIndex_OLD.y; i >= iIndex_YAxis_Proposed; i--)
			{
				vec2Index.y = i;
				if (CheckPosition(DOWN) == false)
				{
					vec2Index.y = vec2OldIndex.y;
					vec2NumMicroSteps.y = 0;
					break;
				}
			}

			if (vec2Index.x >= 0)
			{
				vec2NumMicroSteps.x -= fabs(cos(glm::radians(45.f)) * iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.x <= 0)
				{
					vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
					vec2Index.x--;
				}
			}
			Constraint(LEFT);
			int iIndex_XAxis_Proposed = vec2Index.x;
			for (int i = iIndex_OLD.x; i >= iIndex_XAxis_Proposed; i--)
			{
				vec2Index.x = i;
				if (CheckPosition(LEFT) == false)
				{
					vec2Index.x = vec2OldIndex.x;
					vec2NumMicroSteps.x = 0;
					break;
				}
			}
			break;
		}
		case TOP_RIGHT:
		{
			if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
			{
				vec2NumMicroSteps.y += fabs(sin(glm::radians(45.f)) * iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.y > (int)cSettings->NUM_STEPS_PER_TILE_YAXIS)
				{
					vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
					if (vec2NumMicroSteps.y < 0)
						vec2NumMicroSteps.y = 0;
					vec2Index.y++;
				}
			}
			Constraint(UP);
			int iIndex_YAxis_Proposed = vec2Index.y;
			for (int i = iIndex_OLD.y; i >= iIndex_YAxis_Proposed; i--)
			{
				vec2Index.y = i;
				if (CheckPosition(UP) == false)
				{
					vec2Index.y = vec2OldIndex.y;
					vec2NumMicroSteps.y = 0;
					break;
				}
			}

			if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
			{
				vec2NumMicroSteps.x += fabs(cos(glm::radians(45.f)) * iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.x > (int)cSettings->NUM_STEPS_PER_TILE_XAXIS)
				{
					vec2NumMicroSteps.x -= cSettings->NUM_STEPS_PER_TILE_XAXIS;
					if (vec2NumMicroSteps.x < 0)
						vec2NumMicroSteps.x = 0;
					vec2Index.x++;
				}
			}
			Constraint(RIGHT);
			int iIndex_XAxis_Proposed = vec2Index.x;
			for (int i = iIndex_OLD.x; i >= iIndex_XAxis_Proposed; i--)
			{
				vec2Index.x = i;
				if (CheckPosition(RIGHT) == false)
				{
					vec2Index.x = vec2OldIndex.x;
					vec2NumMicroSteps.x = 0;
					break;
				}
			}
			break;
		}
		case BOTTOM_RIGHT:
		{
			if (vec2Index.y >= 0)
			{
				vec2NumMicroSteps.y -= fabs(sin(glm::radians(45.f)) * iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.y <= 0)
				{
					vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
					vec2Index.y--;
				}
			}
			Constraint(DOWN);
			int iIndex_YAxis_Proposed = vec2Index.y;
			for (int i = iIndex_OLD.y; i >= iIndex_YAxis_Proposed; i--)
			{
				vec2Index.y = i;
				if (CheckPosition(DOWN) == false)
				{
					vec2Index.y = vec2OldIndex.y;
					vec2NumMicroSteps.y = 0;
					break;
				}
			}

			if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
			{
				vec2NumMicroSteps.x += fabs(cos(glm::radians(45.f)) * iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.x > (int)cSettings->NUM_STEPS_PER_TILE_XAXIS)
				{
					vec2NumMicroSteps.x -= cSettings->NUM_STEPS_PER_TILE_XAXIS;
					if (vec2NumMicroSteps.x < 0)
						vec2NumMicroSteps.x = 0;
					vec2Index.x++;
				}
			}
			Constraint(RIGHT);
			int iIndex_XAxis_Proposed = vec2Index.x;
			for (int i = iIndex_OLD.x; i >= iIndex_XAxis_Proposed; i--)
			{
				vec2Index.x = i;
				if (CheckPosition(RIGHT) == false)
				{
					vec2Index.x = vec2OldIndex.x;
					vec2NumMicroSteps.x = 0;
					break;
				}
			}
			break;
		}
		}
		
		cPhysics2D.SetInitialVelocity(cPhysics2D.GetFinalVelocity());
		if (cPhysics2D.GetInitialVelocity().x >= -0.3 && cPhysics2D.GetInitialVelocity().x <= 0.3)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
		}
	}


	//spawn projectile logic
	{
		//replace with mouse control
		
		if (cMouseController->IsButtonDown(GLFW_MOUSE_BUTTON_RIGHT) && cInventoryManager->GetItem("Shivs")->GetCount() > 0)
		{
			if (ProjectileForce < maxPForce)
				ProjectileForce += (3 * dElapsedTime);
			throwing = true;
		}
		else if (cMouseController->IsButtonUp(GLFW_MOUSE_BUTTON_RIGHT) && throwing == true)
		{
			//replace with mouse position
			attackDirection = direction;
			//min force
			if (ProjectileForce > minPForce) {		//throw if force is high enough
				//cSoundController->PlaySoundByID(10);		//replace with fire sound
				//reduce ammo count
				cInventoryItem = cInventoryManager->GetItem("Shivs");
				cInventoryItem->Remove(1);
				//spawn projectile
				CShivs2D* Projectile_Shiv = new CShivs2D();
				Projectile_Shiv->SetShader("Shader2D_Colour");
				if (Projectile_Shiv->Init()) {
					Projectile_Shiv->setDirection(glm::vec2(camera->playerOffset.x, -camera->playerOffset.y));
					EventController::GetInstance()->spawnProjectiles(Projectile_Shiv, getPreciseVec2Index(true));
				}
			}
			//thrown, reset
			throwing = false;
			ProjectileForce = 0;
		}
		else {
			//reset force
			throwing = false;
			ProjectileForce = 0;
		}
	}

	InteractWithMap();

	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}
	

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CPlayer2D::PreRender(void)
{
	// Activate blending mode
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CPlayer2D::Render(void)
{
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtimeColour");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	transform = glm::scale(transform, glm::vec3(camera->zoom, camera->zoom, 0));

	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x + camera->vec2Index.x,
													vec2UVCoordinate.y + camera->vec2Index.y,
													0.0f));

	angle = (atan2(camera->playerOffset.x, camera->playerOffset.y) /3.14159265359) * 180.0;
	transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));


	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(runtimeColour));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	/*
		//Render the Player sprite
		glBindVertexArray(VAO);
		quadMesh->Render();
	*/

	animatedSprites->Render();

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

/**
 @brief Constraint the player's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CPlayer2D::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (vec2Index.x < 0)
		{
			vec2Index.x = 0;
			vec2NumMicroSteps.x = 0;

		}
	}
	else if (eDirection == RIGHT)
	{
		if (vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS - 1);
			vec2NumMicroSteps.x = 0;

		}
	}
	else if (eDirection == UP)
	{
		if (vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS - 1);
			vec2NumMicroSteps.y = 0;

		}
	}
	else if (eDirection == DOWN)
	{
		if (vec2Index.y < 0)
		{
			vec2Index.y = 0;
			vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		cout << "CPLayer::Constraint: Unknown Direction" << endl;
	}
}

void CPlayer2D::setActive(int arr, bool act)
{
	inventorySlots[arr].setAct(act);
}

bool CPlayer2D::getActive(int arr)
{
	return inventorySlots[arr].getAct();
}



bool CPlayer2D::reset_pos()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;

	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game


	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);
	ProjectileForce = 0;

	walkingTime = 0;

	angle = 0;

	direction = RIGHT;

	soundVol = 1.f;

	throwing = false;
	maxPForce = 10;
	minPForce = 5;
	ProjectileForce = 0;

	// vitals
	invincibility = 0;

	dashTrue = true;

	movementSpeed = 1.f;
	attacking = false;
	attackTimer = 0;

	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	return true;
	
}

void CPlayer2D::InteractWithMap(void)
{

	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	/*std::cout << cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 0) << ", "
		<< cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1) << std::endl;*/
		//background switch
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 0)) {
	case 97:		//water
		//disable dash
		dashTrue = false;
		//slow speed
		movementSpeed = 0.5f;
		break;
	case 95:		//spikes
		//deals damage
		LoseHealth(5);
		//slows by abit
		movementSpeed = 0.7f;
		break;

	
	default:
		movementSpeed = 1.f;
		dashTrue = true;
		break;
	}

	//foreground switch
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1)) {
	case 80:		//cross
		if (cKeyboardController->IsKeyDown(GLFW_KEY_E)
			&& ((inventorySlots[0].getitemID() == 89)
				|| (inventorySlots[1].getitemID() == 89 )
				|| (inventorySlots[2].getitemID() == 89))) {
			//shovel the cross to spawn treasures/resources, which will be randomly generated
			int random_generator = rand() % 2 + 1;

			if (random_generator == 2)
			{
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 30, true, 1);
			}
			else
			{
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 40, true, 1);
			}
		}
		break;
	case 79:		//treasure
		if (cKeyboardController->IsKeyDown(GLFW_KEY_E) /*&& key check*/) {
			//open chest to spawn loot
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
		}
		break;
	case 76:		//web
		//slow speed
		//prevent dash
		break;
	case 78:		//dungeon ladderdown
	{
		static bool EInteractDown = false;
		if (cKeyboardController->IsKeyDown(GLFW_KEY_E) && !EInteractDown) {
			EInteractDown = true;
			cMap2D->SetCurrentLevel(1);
			uiRow = -1; uiCol = -1;
			if (cMap2D->FindValue(77, uiRow, uiCol) == true)
			{
				vec2Index.x = uiCol;
				vec2Index.y = uiRow;
			}
			else {
				cMap2D->SetCurrentLevel(0);
			}
		}
		else if (!cKeyboardController->IsKeyDown(GLFW_KEY_E) && EInteractDown) {
			EInteractDown = false;
		}
		//cGameManager->bLevelIncrease = true;
		break;
	}
	case 77:		//dungeon ladderup
	{
		static bool EInteractDown = false;
		if (cKeyboardController->IsKeyDown(GLFW_KEY_E) && !EInteractDown) {
			EInteractDown = true;
			cMap2D->SetCurrentLevel(0);
			uiRow = -1; uiCol = -1;
			if (cMap2D->FindValue(78, uiRow, uiCol) == true)
			{
				vec2Index.x = uiCol;
				vec2Index.y = uiRow;
			}
			else {
				cMap2D->SetCurrentLevel(1);
			}
		}
		else if (!cKeyboardController->IsKeyDown(GLFW_KEY_E) && EInteractDown) {
			EInteractDown = false;
		}
		//cGameManager->bLevelDecrease = true;
		break;
	}

	//PICKING UP ITEMS
	case 70:
	case 81:
	case 30:
	case 49:
	case 40:
	case 39:
	case 38:
	case 37:
	case 36:
	case 35:
	case 34:
	case 33:
	case 90:
	case 50:
		AddItem(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1));
		break;
	case 51:
	{
		AddItem(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1));
		break;
	}
	case 10:
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->replaceBlade(new CDaggerBlade2D());
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
		break;
	}
	case 11:
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->replaceBlade(new CCleaverBlade2D());
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
		break;
	}
	case 12:
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->replaceBlade(new CKatanaBlade2D());
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
		break;
	}
	case 20:
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->replaceHilt(new CWoodenHilt2D());
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
		break;
	}
	case 21:
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->replaceHilt(new CIronHilt2D());
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
		break;
	}
	case 22:
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->replaceHilt(new CPlatinumHilt2D());
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
		break;
	}
	default:
		//reset speed
		//reset dash true
		break;
	}

}

void CPlayer2D::AttackEnemy()
{
	vector<CEnemy2D*> enemies = EventController::GetInstance()->enemyVector;
	for (CEnemy2D* enemy : enemies)
	{
		if (enemy->bIsActive)
		{
			CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
			float enemyAngle = (atan2((enemy->getPreciseVec2Index(true).y - getPreciseVec2Index(true).y),
				(enemy->getPreciseVec2Index(true).x - getPreciseVec2Index(true).x)) / 3.14159265359) * 180.0f;
			if (enemyAngle - angle + 90 > 270)
				enemyAngle -= 360;
			if (cPhysics2D.CalculateDistance(getPreciseVec2Index(true),
				enemy->getPreciseVec2Index(true)) <= sword->getTotalRange() + 1.5f &&
				enemyAngle - angle + 90 >= -40 + sword->getTotalRange() * 2 &&
				enemyAngle - angle + 90 <= 40 + sword->getTotalRange() * 2)
			{
				if (!enemy->sleep)
				{
					enemy->takeDamage(sword->getTotalDamage());
					if (sword->getEffect() != AILMENT::NONE)
						enemy->SetStatus(sword->getEffect(), 3.f);
				}
				else if (enemy->sleep)
					enemy->sleep = false;
				CInventoryManager::GetInstance()->GetItem("Health")->Add(sword->getTotalRavenous());
				cSoundController->PlaySoundByID(12);
			}
		}
	}
}

bool CPlayer2D::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (vec2NumMicroSteps.y == 0)
		{
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
				return false;
		}
		else if (vec2NumMicroSteps.y != 0)
		{
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x)) >= 100 || (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x)>= 100))
				return false;
		}
	}
	
	else if (eDirection == RIGHT)
	{
		if (vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2NumMicroSteps.x = 0;
			return true;
		}
		if (vec2NumMicroSteps.y == 0)
		{
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100)
				return false;
		}
		else if (vec2NumMicroSteps.y != 0)
		{
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1)) >= 100 || (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
				return false;
		}
	}

	else if (eDirection == UP)
	{
		if (vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2NumMicroSteps.y = 0;
			return true;
		}
		if (vec2NumMicroSteps.x == 0)
		{
			if (cMap2D->GetMapInfo(vec2Index.y+1, vec2Index.x) >= 100)
				return false;
		}
		else if (vec2NumMicroSteps.x != 0)
		{
			if ((cMap2D->GetMapInfo(vec2Index.y+1, vec2Index.x)) >= 100 || (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x+1) >= 100))
				return false;
		}
	}

	else if (eDirection == DOWN)
	{
		if (vec2NumMicroSteps.x == 0)
		{
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
				return false;
		}
		else if (vec2NumMicroSteps.x != 0)
		{
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x)) >= 100 || (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x+1) >= 100))
				return false;
		}
	}
	else
	{
		cout << "CPlayer2D::CheckPosition: Unknown direction." << endl;
	}


	return true;
}

void CPlayer2D::UpdateHealthLives(void)
{
	cInventoryItem = cInventoryManager->GetItem("Health");

	if (cInventoryItem->GetCount() <= 0)
	{
		cInventoryItem->iItemCount = cInventoryItem->GetMaxCount();

		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Remove(1);

		if (cInventoryItem->GetCount() < 0)
		{
			CGameManager::GetInstance()->bPlayerLost = true;
		}
	}
}

CPlayer2D::DIRECTION CPlayer2D::getAttackDirection(void)
{
	return attackDirection;
}

double CPlayer2D::getProjectileForce()
{
	return ProjectileForce;
}

void CPlayer2D::LoseHealth(float health)
{
	if (invincibility == 0) {
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(health);
		invincibility = 0.5f;
	}
}

bool CPlayer2D::getAttacking()
{
	return attacking;
}

bool CPlayer2D::AddItem(int itemid)
{
	for (int i = 0; i < 9; i++)
	{
		//IF THE SLOT IS EMPTY
		if (inventorySlots[i].getitemID() == 0 )
		{
			inventorySlots[i].setitemID(itemid);
			inventorySlots[i].settextureID(itemid);
			inventorySlots[i].AddQuantity(1);
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
			break;
		}
		//IF THE SLOT IS FULL
		else
		{
			//IF ITEM IS NOT A NON STACKABLE ITEM (IE CAMPFIRE)
			//IF THE DECLARED ITEMID IN THE SLOT IS THE SAME AS THE ITEM PICKED UP
			if (inventorySlots[i].getitemID() == itemid
				&& (inventorySlots[i].getitemID() != 50))
			{
				//IF THE QUANTITY IS BELOW 5
				if (inventorySlots[i].getquantity() < 5)
				{
					cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
					inventorySlots[i].AddQuantity(1);
					break;
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
			
		}

		if (i < 3)
		{
			return false;
		}
	}
	return true;
}

slot CPlayer2D::getitem(int arr)
{
	return inventorySlots[arr];
}

void CPlayer2D::setitem(int arr, int itemid)
{
	inventorySlots[arr].setitemID(itemid);
	inventorySlots[arr].settextureID(itemid);
	
}

void CPlayer2D::setitemquantity(int arr, int quantity)
{
	inventorySlots[arr].setquantity(quantity);
}

int CPlayer2D::getitemval(int arr)
{
	return inventorySlots[arr].getitemID();
}

int CPlayer2D::gettextureid(int arr)
{
	return inventorySlots[arr].gettextureID();
}

int CPlayer2D::getOctopusKillCount()
{
	return octopusKillCount;
}

int CPlayer2D::getSpiderKillCount()
{
	return spiderKillCount;
}

int CPlayer2D::getChickenKillCount()
{
	return chickenKillCount;
}

int CPlayer2D::getSkeletonKillCount()
{
	return skeletonKillCount;
}

void CPlayer2D::setsound(float vol)
{
	soundVol = vol;
}

float CPlayer2D::returnsound()
{
	return soundVol;
}

void CPlayer2D::addOctopusKillCount(int kill)
{
	octopusKillCount += kill;
}

void CPlayer2D::addSpiderKillCount(int kill)
{
	spiderKillCount += kill;
}

void CPlayer2D::addChickenKillCount(int kill)
{
	chickenKillCount += kill;
}
void CPlayer2D::addSkeletonKillCount(int kill)
{
	skeletonKillCount += kill;
}

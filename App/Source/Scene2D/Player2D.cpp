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

#include "Sword2D.h"
#include "WoodenHilt2D.h"
#include "RustyBlade2D.h"
#include "CleaverBlade2D.h"

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
	, soundsfx(NULL)
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

	if (soundsfx)
	{
		delete soundsfx;
		soundsfx = NULL;
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
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol, true, 1) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	ProjectileForce = 0;

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

	cMap2D->SetMapInfo(uiRow, uiCol, 0, true, 1);			//replace player with sand cause they spawn on sand

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Load the player texture 
	/*
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/scene2D_player.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_PlayerTile.tga" << endl;
		return false;
	}
	*/
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
	

	/*animatedSprites = CMeshBuilder::GenerateSpriteAnimation(2, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idleLeft", 2, 2);
	animatedSprites->AddAnimation("idleRight", 0, 0);
	animatedSprites->AddAnimation("walkLeft", 2, 3);
	animatedSprites->AddAnimation("walkRight", 0, 1);
	animatedSprites->AddAnimation("jumpLeft", 3, 3);
	animatedSprites->AddAnimation("jumpRight", 1, 1);

	animatedSprites->AddAnimation("idleLeftSW", 6, 6);
	animatedSprites->AddAnimation("idleRightSW", 4, 4);
	animatedSprites->AddAnimation("walkLeftSW", 6, 7);
	animatedSprites->AddAnimation("walkRightSW", 4, 5);
	animatedSprites->AddAnimation("jumpLeftSW", 7, 7);
	animatedSprites->AddAnimation("jumpRightSW", 5, 5);

	animatedSprites->PlayAnimation("idleRightSW", -1, 0.3f);*/

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

	//debug shivs
	cInventoryItem = cInventoryManager->Add("Shivs", "Image/Scene2D_Health.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	//Add ITEMS
	cInventoryItem = cInventoryManager->Add("Stick", "Image/Sp3Images/Base/stick.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	cInventoryItem = cInventoryManager->Add("Wood", "Image/Sp3Images/Base/wood.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	cInventoryItem = cInventoryManager->Add("Swords", "Image/Sp3Images/Weapons/sword.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	CSword2D* sword = new CSword2D(new CWoodenHilt2D(), new CCleaverBlade2D());
	cInventoryManager->Add(sword);

	//sword->replaceBlade(new CRustyBlade2D());

	cInventoryItem->vec2Size = glm::vec2(25, 25);
	cSoundController = CSoundController::GetInstance();

	il = CImageLoader::GetInstance();
	
	//MUST CONSIDER THESE POSSIBLE CASES
	//1. SLOT REACHED MAX LIMIT
	//2. ITEM PICKED UP IS A DIFFERENT ITEM
	//3. IF ITEM IS EMPTIED
	//if(itemid == inventoryslotitemid
	//if(maxcount reached
	//id count ==0

	//set inventory slots to 0 at the start of the game
	for (int i = 0; i < 9; i++)
	{
		//inventorySlots[i].setitemID(0);
		if (i % 2 == 0)
		{
			inventorySlots[i].setitemID(30);
		}
		else
		{
			inventorySlots[i].setitemID(40);
		}

		inventorySlots[i].settextureID(inventorySlots[i].getitemID());
		
		cout << "MAP " << i << " IS " << inventorySlots[i].gettextureID() << endl;


		inventorySlots[i].AddQuantity(5);
		//inventory
		/*{
			inventorySlots[i].AddQuantity(1);

			inventorySlots[i].AddQuantity(1);

		}*/

		//cout << "PLAYER 2D " << i << " IS " << inventorySlots[i].gettextureID() << endl;
	}

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
	//for (int i = 0; i < 9; i++)
	//{
	//	cout << "PA" << i << " is " << getitemval(i) << endl;
	//	//cout << "player array" << i << " is " << guiscene2d->return_hbcellid(i) << endl;
	//}
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
	// Store the old position

	if (cKeyboardController->IsKeyDown(GLFW_KEY_A)
		|| cKeyboardController->IsKeyDown(GLFW_KEY_S)
		|| cKeyboardController->IsKeyDown(GLFW_KEY_D)
		|| cKeyboardController->IsKeyDown(GLFW_KEY_W))
	{

		//switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1))
		//{
		//	case 99: //grass
		//	{
		ISound* grassSound = cSoundController->PlaySoundByID_2(7);
		if (grassSound != nullptr)
		{
			soundsfx = grassSound;
		}
		if (soundsfx != nullptr)
		{
			soundsfx->setVolume(soundVol);
		}
		//		break;
		//	}
		//	case 98: //sand
		//	{
		//		ISound* sandSound = cSoundController->PlaySoundByID_2(8);
		//		if (sandSound != nullptr)
		//		{
		//			soundsfx = sandSound;
		//		}
		//		if (soundsfx != nullptr)
		//		{
		//			soundsfx->setVolume(soundVol);
		//		}
		//		break;
		//	}
		//	case 97: //water
		//	{
		//		ISound* waterSound = cSoundController->PlaySoundByID_2(9);
		//		if (waterSound != nullptr)
		//		{
		//			soundsfx = waterSound;
		//		}
		//		if (soundsfx != nullptr)
		//		{
		//			soundsfx->setVolume(soundVol);
		//		}
		//		break;
		//	}
		//	default:
		//	{
		//		break;
		//	}
		//}

	}
	vec2OldIndex = vec2Index;

	// vitals
	static float hungerTimer = 0;
	hungerTimer += dElapsedTime;

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

	//std::cout << "Hunger: " << cInventoryManager->GetItem("Hunger")->GetCount() << std::endl;
	//std::cout << "Health: " << cInventoryManager->GetItem("Health")->GetCount() << std::endl;

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

			runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
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

			runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

			/*if (hasSword || chargeSword)
				animatedSprites->PlayAnimation("walkLeftSW", -1, 0.1f);
			else
				animatedSprites->PlayAnimation("walkLeft", -1, 0.15f);*/

			//angle = 0;
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

			runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
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

			runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
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
				soundsfx = dodgeSound;
			}
			if (soundsfx != nullptr)
			{
				soundsfx->setVolume(soundVol);
			}
			
		}
		else if (!cKeyboardController->IsKeyDown(GLFW_KEY_SPACE) && !cKeyboardController->IsKeyDown(GLFW_KEY_LEFT_SHIFT) && dodgeKeyDown)
			dodgeKeyDown = false;
	}
	//std::cout << cInventoryManager->GetItem("Stamina")->GetCount() << std::endl;
	//std::cout << cInventoryManager->GetItem("Stamina")->GetMaxCount() << std::endl;
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::DODGE)
	{
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

void CPlayer2D::InteractWithMap(void)
{
	/*std::cout << cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 0) << ", "
		<< cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1) << std::endl;*/
		//background switch
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 0))
	{
	case 97:		//water
		//disable dash
		dashTrue = false;
		//slow speed
		movementSpeed = 0.5f;
		break;

	case 93:		//spikes
		//deals damage
		LoseHealth(5);
		//slows by abit
		movementSpeed = 0.9f;
		break;

	//PICKING UP ITEMS
	/*case 30:
	case 40:
		for (int i = 0; i < 9; i++)
		{
			if (inventorySlots[i].getitemID() == 0)
			{
				AddItem(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1));
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
				break;

			}
		}
		break;*/

	default:
		movementSpeed = 1.f;
		dashTrue = true;
		break;
	}

	//foreground switch
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1))
	{

	case 80:		//cross
		if (cKeyboardController->IsKeyDown(GLFW_KEY_E) /*&& shovelcheck*/) {
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
		if (cKeyboardController->IsKeyDown(GLFW_KEY_E) /*&& key check*/ ) {
			//open chest to spawn loot
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0, true, 1);
		}
		break;
	case 78:		//dungeon ladderdown
		//add level
		cGameManager->bLevelIncrease = true;
		break;

	case 77:		//dungeon ladderup
		//remove level
		cGameManager->bLevelDecrease = true;
		break;
	case 76:		//web
		//slow speed
		//prevent dash
		break;
	default:
		//reset speed
		//reset dash true
		break;
	}




	//forage tree
	//if (vec2Index-cMap2D->GetMapInfo(vec2Index.x, vec2Index.y, 100) < 1)
	//{
	//	if (cKeyboardController->IsKeyDown(GLFW_KEY_E) /*&& shovelcheck*/) {

	//	}
	//}


	/*switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x, true, 1))
	{

	}*/
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

		if (inventorySlots[i].getquantity() == 5)
		{
			inventorySlots[i+1].setitemID(itemid);
			inventorySlots[i+1].settextureID(itemid);
		}
		else
		{
			inventorySlots[i].AddQuantity(1);
		}


		if (inventorySlots[i].getitemID() == 0)
		{
			

			inventorySlots[i].setitemID(itemid);
			inventorySlots[i].settextureID(itemid);

			if (i < 3)
			{
				return false;
			}
			break;
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
	//inventorySlots[arr].loadimagebasedID(inventorySlots[arr].getitemID(), il);

}

int CPlayer2D::getitemval(int arr)
{
	return inventorySlots[arr].getitemID();
}

int CPlayer2D::gettextureid(int arr)
{
	return inventorySlots[arr].gettextureID();
}

void CPlayer2D::setsound(float vol)
{
	soundVol = vol;
}

float CPlayer2D::returnsound()
{
	return soundVol;
}

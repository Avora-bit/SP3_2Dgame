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

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
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
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
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

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	camera = Camera::GetInstance();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	iJumpCount = 0;

	hasSword = true;
	slash = false;
	chargeSword = false;
	swordForce = 0;

	direction = RIGHT;

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

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
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load the player texture
	// Load the ground texture

	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/charSprite.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}


	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(2, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
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

	animatedSprites->PlayAnimation("idleRightSW", -1, 0.3f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	cInventoryManager = CInventoryManager::GetInstance();

	cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Bouquet", "Image/bouquet.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cSoundController = CSoundController::GetInstance();

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

	iJumpCount = 0;

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	return true;
}

/**
 @brief Update this instance
 */
void CPlayer2D::Update(const double dElapsedTime)
{
	// Store the old position
	vec2OldIndex = vec2Index;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_R))
		Reset();

	if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
	{
		if (vec2Index.x >= 0)
		{
			vec2NumMicroSteps.x--;
			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
			}
		}

		if (!CheckPosition(LEFT))
		{
			vec2Index = vec2OldIndex;
			vec2NumMicroSteps.x = 0;
		}

		runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
		Constraint(LEFT);

		if (hasSword || chargeSword)
			animatedSprites->PlayAnimation("walkLeftSW", -1, 0.1f);
		else
			animatedSprites->PlayAnimation("walkLeft", -1, 0.15f);

		direction = LEFT;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
	{
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			vec2NumMicroSteps.x++;
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

		if (hasSword || chargeSword)
			animatedSprites->PlayAnimation("walkRightSW", -1, 0.1f);
		else
			animatedSprites->PlayAnimation("walkRight", -1, 0.15f);

		direction = RIGHT;
	}
	else
	{
		switch (direction)
		{
		case LEFT:
			if (hasSword || chargeSword)
				animatedSprites->PlayAnimation("idleLeftSW", -1, -1.0f);
			else
				animatedSprites->PlayAnimation("idleLeft", -1, -1.0f);
			break;
		case RIGHT:
			if (hasSword || chargeSword)
				animatedSprites->PlayAnimation("idleRightSW", -1, -1.0f);
			else
				animatedSprites->PlayAnimation("idleRight", -1, -1.0f);
			break;
		}
	}

	if (chargeSword)
	{
		if (cKeyboardController->IsKeyDown(GLFW_KEY_ENTER))
		{
			if (swordForce < 7)
				swordForce += (dElapsedTime * 10);
		}
		else
		{
			attackDirection = direction;
			chargeSword = false;
			if (swordForce < 1.5)
			{
				cSoundController->PlaySoundByID(9);
				hasSword = true;
				slash = true;
			}
			else
				cSoundController->PlaySoundByID(10);
		}
	}
	
	if (hasSword && cKeyboardController->IsKeyDown(GLFW_KEY_ENTER))
	{
		swordForce = 0;
		chargeSword = true;
		hasSword = false;
	}

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_SPACE))
	{
		if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
			cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.0f));
			iJumpCount += 1;

			cSoundController->PlaySoundByID(6);
		}
		else
		{
			if (iJumpCount < 2)
			{
				cSoundController->PlaySoundByID(6);

				cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
				cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.0f));
				iJumpCount += 1;
			}
		}
	}
	if (IsMidAir())
	{
		if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
	}

	if (cInventoryManager->GetItem("Health")->GetCount() <= 0)
		CGameManager::GetInstance()->bPlayerLost = true;

	UpdateJumpFall(dElapsedTime);
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
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x))
	{
	case 2:
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);

		cInventoryItem = cInventoryManager->GetItem("Bouquet");
		cInventoryItem->Add(1);

		cSoundController->PlaySoundByID(5);
		break;
	case 20:
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(1);
		break;
	case 22:
		cInventoryItem = cInventoryManager->GetItem("Health");
		if (cInventoryItem->GetCount() < cInventoryItem->GetMaxCount())
		{
			cSoundController->PlaySoundByID(5);

			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
			cInventoryItem->Add(50);
		}
		break;
	case 98:
		cInventoryItem = cInventoryManager->GetItem("Bouquet");
		if (cInventoryItem->GetCount() == cInventoryItem->GetMaxCount())
		{
			cSoundController->PlaySoundByID(7);
			if (!hasSword)
				hasSword = true;
			CGameManager::GetInstance()->bLevelCompleted = true;
		}
		break;
	case 99:
		CGameManager::GetInstance()->bPlayerWon = true;
		break;
	default:
		break;
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

void CPlayer2D::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	{
		cPhysics2D.SetTime((float)dElapsedTime);
		cPhysics2D.Update();
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();
		int iIndex_YAxis_OLD = vec2Index.y;
		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS);
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += iDisplacement_MicroSteps;
			if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
				if (vec2NumMicroSteps.y < 0)
					vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}
		Constraint(UP);
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
		{
			vec2Index.y = i;
			if (CheckPosition(UP) == false)
			{
				vec2NumMicroSteps.y = 0;
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				break;
			}
		}
		if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.GetDisplacement().y <= 0.0f))
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		cPhysics2D.SetTime((float)dElapsedTime);
		cPhysics2D.Update();
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();
		int iIndex_YAxis_OLD = vec2Index.y;
		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS);
		{
			if (vec2Index.y >= 0)
			{
				vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
				if (vec2NumMicroSteps.y < 0)
				{
					vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
					vec2Index.y--;
				}
			}
		}
		Constraint(DOWN);
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
		{
			vec2Index.y = i;
			if (CheckPosition(DOWN) == false)
			{
				if (i != iIndex_YAxis_OLD)
					vec2Index.y = i + 1;
				cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
				iJumpCount = 0;
				vec2NumMicroSteps.y = 0;
				break;
			}
		}
	}

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

bool CPlayer2D::IsMidAir(void)
{
	if (vec2Index.y == 0)
		return false;

	if ((vec2NumMicroSteps.x == 0) &&
		(cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) <= 100))
		return true;
	return false;
}

CPlayer2D::DIRECTION CPlayer2D::getAttackDirection(void)
{
	return attackDirection;
}

bool CPlayer2D::getChargeSword()
{
	return chargeSword;
}

double CPlayer2D::getSwordForce()
{
	return swordForce;
}

void CPlayer2D::LoseHealth(float health)
{
	cInventoryItem = cInventoryManager->GetItem("Health");
	cInventoryItem->Remove(health);
}

bool CPlayer2D::getHasSword(void)
{
	return hasSword;
}

void CPlayer2D::SetHasSword(bool hasSword)
{
	this->hasSword = hasSword;
}

bool CPlayer2D::getSlash(void)
{
	return slash;
}

void CPlayer2D::setSlash(bool slash)
{
	this->slash = slash;
}
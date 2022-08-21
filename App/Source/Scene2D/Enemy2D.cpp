/**
 CEnemy2D
 @brief A class which represents the enemy object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Enemy2D.h"

#include <iostream>
using namespace std;

// Include GLEW
#include <GL/glew.h>

// Include ImageLoader
#include "System\ImageLoader.h"

// Include math.h
#include <math.h>

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::CEnemy2D(void)
	: bIsActive(false)
	, cMap2D(NULL)
	, cSettings(NULL)
	, cPlayer2D(NULL)
	, CShivs2D(NULL)
	, sCurrentFSM(FSM::IDLE)
	, iFSMCounter(0)
{
	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	vec2Destination = glm::vec2(0, 0);	// Initialise the iDestination
	vec2Direction = glm::vec2(1, 0);		// Initialise the iDirection
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::~CEnemy2D(void)
{
	// We won't delete this since it was created elsewhere
	cPlayer2D = NULL;

	CShivs2D = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	camera = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  */
bool CEnemy2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	camera = Camera::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	CShivs2D = CShivs2D::GetInstance();

	cPlayer2D = CPlayer2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(300, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	//quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/enemySprite.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load enemy tile texture" << std::endl;
		return false;
	}

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 2, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idleLeft", 1, 1);
	animatedSprites->AddAnimation("idleRight", 0, 0);

	animatedSprites->PlayAnimation("idleRight", -1, 0.3f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	timer = 0;

	health = 40;

	return true;
}

/**
 @brief Update this instance
 */
void CEnemy2D::Update(const double dElapsedTime)
{
	timer += dElapsedTime;

	if (!bIsActive)
		return;

	if (health <= 0)
		bIsActive = false;

	/*if (cPlayer2D->getSlash())
	{
		static float dmgTimer = 0;
		dmgTimer += dElapsedTime;
		if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) <= 1.0f && dmgTimer >= 0.1)
		{
			health -= 20;
			dmgTimer = 0;
		}
	}*/

	/*if (!CShivs2D->thrown && !cPlayer2D->getHasSword() && CShivs2D->vec2Index == vec2Index && CShivs2D->vec2Index != cPlayer2D->vec2Index)
	{
		health -= 40;
	}*/
	switch (sCurrentFSM)
	{
	case IDLE:
		if (iFSMCounter > iMaxFSMCounter)
		{ 
			// if player encounter
			if ((cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 5.0f &&
				((vec2Direction.x < 0 && vec2Index.x - cPlayer2D->vec2Index.x < 0) ||
				 (vec2Direction.x >= 0 && vec2Index.x - cPlayer2D->vec2Index.x >= 0)))||
				 cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 3.0f)
			{
				timer = 0;
				sCurrentFSM = CHASE;
				iFSMCounter = 0;

			}
			else if (timer >= 2.5) // after 3s idle, go into patrol
			{
				timer = 0;
				sCurrentFSM = PATROL;
				iFSMCounter = 0;
				//cout << "Switching to Patrol State" << endl;
			}
		}
		iFSMCounter++;
		break;
	case PATROL:
	{
		static float jumpCooldown = 0;
		jumpCooldown += dElapsedTime;
		if (iFSMCounter > iMaxFSMCounter)
		{ // after 2s patrol, go back into idle
			if (timer >= 1.5)
			{
				timer = 0;
				sCurrentFSM = IDLE;
				iFSMCounter = 0;
				break;
			}
		} // if player spotted, go into chase
		else if ((cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 5.0f &&
			((vec2Direction.x < 0 && vec2Index.x - cPlayer2D->vec2Index.x < 0) ||
				(vec2Direction.x >= 0 && vec2Index.x - cPlayer2D->vec2Index.x >= 0))) ||
			cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 3.0f)
		{
			timer = 0;
			sCurrentFSM = CHASE;
			iFSMCounter = 0;
			break;
		} // move around
		//else
		//{
		//	// Patrol around
		//	// Update the Enemy2D's position for patrol
		//	//UpdatePosition();
		//	// Store the old position
		//	vec2OldIndex = vec2Index;
		//	if (vec2Direction.x < 0)
		//	{
		//		// Move left
		//		const int iOldIndex = vec2Index.x;
		//		if (vec2Index.x >= 0)
		//		{
		//			vec2NumMicroSteps.x--;
		//			if (vec2NumMicroSteps.x < 0)
		//			{
		//				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
		//				vec2Index.x--;
		//			}
		//		}
		//		// Constraint the enemy2D's position within the screen boundary
		//		Constraint(LEFT);
		//		// Find a feasible position for the enemy2D's current position
		//		if (CheckPosition(LEFT) == false)
		//		{
		//			// if blocks can be jumped above, jump (2 blocks leeway)
		//			if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE && jumpCooldown >= 1)
		//			{
		//				if (iFSMCounter > iMaxFSMCounter)
		//				{
		//					timer = 0;
		//					sCurrentFSM = IDLE;
		//					iFSMCounter = 0;
		//					//cout << "Switching to Idle State" << endl;
		//					break;
		//				}
		//			}
		//			else
		//			{
		//				vec2Index = vec2OldIndex;
		//				vec2NumMicroSteps.x = 0;
		//			}
		//		}
		//		// Interact with the Player
		//		//InteractWithPlayer();
		//	}
		//	else if (vec2Direction.x > 0)
		//	{
		//		// Move right
		//		const int iOldIndex = vec2Index.x;
		//		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		//		{
		//			vec2NumMicroSteps.x++;
		//			if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
		//			{
		//				vec2NumMicroSteps.x = 0;
		//				vec2Index.x++;
		//			}
		//		}
		//		// Constraint the enemy2D's position within the screen boundary
		//		Constraint(RIGHT);
		//		// Find a feasible position for the enemy2D's current position
		//		if (CheckPosition(RIGHT) == false)
		//		{
		//			// if blocks can be jumped above, jump (2 blocks leeway)
		//			if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE && jumpCooldown >= 1)
		//			{
		//				jumpCooldown = 0;
		//				cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
		//				cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.0f));
		//				if (iFSMCounter > iMaxFSMCounter)
		//				{
		//					timer = 0;
		//					sCurrentFSM = IDLE;
		//					iFSMCounter = 0;
		//					//cout << "Switching to Idle State" << endl;
		//					break;
		//				}
		//			}
		//			else
		//			{
		//				vec2Index = vec2OldIndex;
		//				vec2NumMicroSteps.x = 0;
		//			}
		//			// Check if enemy2D is in mid-air, such as walking off a platform
		//			// Interact with the Player
		//			//InteractWithPlayer();
		//		}
		//		// if the player is above the enemy2D, then jump to attack
		//		if (vec2Direction.y > 0)
		//		{
		//			if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
		//			{
		//				cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
		//				cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.0f));
		//			}
		//		}
		//	}
		//	else
		//	{
		//		if (CheckPosition(LEFT))
		//			vec2Direction.x = -1;
		//		else if (CheckPosition(RIGHT))
		//			vec2Direction.x = 1;
		//	}
		//}
		iFSMCounter++;
		break;
	}
	case CHASE:
		if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 5.0f)
		{
			vec2Direction = vec2Index-cPlayer2D->vec2Index;
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 1.0f)
			{
				if (iFSMCounter > iMaxFSMCounter)
				{
					timer = 0;
					sCurrentFSM = ATTACK;
					iFSMCounter = 0;
					//cout << "atk" << endl;
					break;
				}
			}
			//cout << "vec2Destination : " << vec2Destination.x 
			//		<< ", " << vec2Destination.y << endl;
			//cout << "vec2Direction : " << vec2Direction.x 
			//		<< ", " << vec2Direction.y << endl;
			//system("pause");

			// Attack
			// Update direction to move towards for attack
			//UpdateDirection();

			// Update the Enemy2D's position for attack

			//if (timer >= 0.1)
			//{
			//	timer = 0;
				//cMap2D->PrintSelf();
				//cout << "StartPos: " << vec2Index.x << ", " << vec2Index.y << endl;
				//cout << "TargetPos: " << cPlayer2D->vec2Index.x << ", " << cPlayer2D->vec2Index.y << endl;
			auto path = cMap2D->PathFind(vec2Index,
				cPlayer2D->vec2Index, heuristic::manhattan, 10);

			bool bFirstPosition = true;
			for (const auto& coord : path)
			{
				if (bFirstPosition == true)
				{
					vec2Destination = coord;

					vec2Direction = vec2Destination - vec2Index;
					bFirstPosition = false;
				}
				else
				{
					if ((coord - vec2Destination) == vec2Direction)
					{
						vec2Destination = coord;
					}
					else
						break;
				}
			}
			UpdatePosition();
			//}
		}
		else
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				sCurrentFSM = PATROL;
				iFSMCounter = 0;
				timer = 0;
				//cout << "ATTACK : Reset counter: " << iFSMCounter << endl;
				break;
			}
		}
		iFSMCounter++;
		break;
	case ATTACK:
	{
		static float attackTimer = 0;
		attackTimer += dElapsedTime;
		if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) <= 1.0f)
		{
			if (attackTimer >= 1)
			{
				cPlayer2D->LoseHealth(15);
				attackTimer = 0;
			}
		}
		else
		{
			sCurrentFSM = PATROL;
			iFSMCounter = 0;
			timer = 0;
			break;
		}
		iFSMCounter++;
		break;
	}
	default:
		break;
	}

	animatedSprites->Update(dElapsedTime);
	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CEnemy2D::PreRender(void)
{
	if (!bIsActive)
		return;

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CEnemy2D::Render(void)
{
	if (!bIsActive)
		return;

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

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// Render the tile
	animatedSprites->Render();

	glBindVertexArray(0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CEnemy2D::PostRender(void)
{
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}

/**
 @brief Set the handle to cPlayer to this class instance
 @param cPlayer2D A CPlayer2D* variable which contains the pointer to the CPlayer2D instance
 */
void CEnemy2D::SetPlayer2D(CPlayer2D* cPlayer2D)
{
	this->cPlayer2D = cPlayer2D;

	// Update the enemy's direction
	UpdateDirection();
}


/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CEnemy2D::Constraint(DIRECTION eDirection)
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
			vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
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
		cout << "CEnemy2D::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CEnemy2D::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == RIGHT)
	{
		// If the new position is at the top row, then return true
		if (vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}

	}
	else if (eDirection == UP)
	{
		// If the new position is at the top row, then return true
		if (vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else
	{
		cout << "CEnemy2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

/**
 @brief Let enemy2D interact with the player.
 */
bool CEnemy2D::InteractWithPlayer(void)
{
	glm::vec2 vec2PlayerPos = cPlayer2D->vec2Index;
	
	// Check if the enemy2D is within 1.5 indices of the player2D
	if (((vec2Index.x >= vec2PlayerPos.x - 0.5) && 
		(vec2Index.x <= vec2PlayerPos.x + 0.5))
		&& 
		((vec2Index.y >= vec2PlayerPos.y - 0.5) &&
		(vec2Index.y <= vec2PlayerPos.y + 0.5)))
	{
		cout << "Gotcha!" << endl;
		// Since the player has been caught, then reset the FSM
		sCurrentFSM = IDLE;
		iFSMCounter = 0;
		return true;
	}
	return false;
}

/**
 @brief Update the enemy's direction.
 */
void CEnemy2D::UpdateDirection(void)
{
	// Set the destination to the player
	vec2Destination = cPlayer2D->vec2Index;

	// Calculate the direction between enemy2D and player2D
	vec2Direction = vec2Destination - vec2Index;

	// Calculate the distance between enemy2D and player2D
	float fDistance = cPhysics2D.CalculateDistance(vec2Index, vec2Destination);
	if (fDistance >= 0.01f)
	{
		// Calculate direction vector.
		// We need to round the numbers as it is easier to work with whole numbers for movements
		vec2Direction.x = (int)round(vec2Direction.x / fDistance);
		vec2Direction.y = (int)round(vec2Direction.y / fDistance);
	}
	else
	{
		// Since we are not going anywhere, set this to 0.
		vec2Direction = glm::vec2(0);
	}
}

/**
@brief Update position.
*/
void CEnemy2D::UpdatePosition(void)
{
	// Store the old position
	vec2OldIndex = vec2Index;
	if (vec2Direction.y < 0)
	{
		// Move left
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y -= speed_multiplier;
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(DOWN);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(DOWN) == false)
		{
			vec2Index = vec2OldIndex;
			vec2NumMicroSteps.x = 0;
		}

		// Interact with the Player
		//InteractWithPlayer();
	}
	else if (vec2Direction.y > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += speed_multiplier;

			if (vec2NumMicroSteps.y >= cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(UP);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(UP) == false)
		{
			//vec2Index = vec2OldIndex;
			vec2NumMicroSteps.x = 0;
		}


		// Interact with the Player
		//InteractWithPlayer();
	}
	if (vec2Direction.x < 0)
	{
		// Move left
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x >= 0)
		{
			vec2NumMicroSteps.x -= speed_multiplier;
			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(LEFT);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(LEFT) == false)
		{
			vec2Index = vec2OldIndex;
			vec2NumMicroSteps.x = 0;
		}
		
		// Interact with the Player
		//InteractWithPlayer();
	}
	else if (vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			vec2NumMicroSteps.x += speed_multiplier;

			if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				vec2NumMicroSteps.x = 0;
				vec2Index.x++;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(RIGHT);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(RIGHT) == false)
		{
			//vec2Index = vec2OldIndex;
			vec2NumMicroSteps.x = 0;
		}

		
		// Interact with the Player
		//InteractWithPlayer();
	}
}

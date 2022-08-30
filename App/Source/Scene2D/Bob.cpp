#include "Bob.h"

#include "EventController.h"

Bob::Bob()
{
	health = 100;
	atk = 10;
}

Bob::Bob(glm::vec2 pos)
{
	vec2Index = pos;
	health = 100;
	atk = 10;
}

Bob::~Bob()
{
	// We won't delete this since it was created elsewhere
	cPlayer2D = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	camera = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool Bob::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	camera = Camera::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	cPlayer2D = CPlayer2D::GetInstance();

	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	//quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/Enemies/bobSprite.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load Bob tile texture" << std::endl;
		return false;
	}

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("default", 0, 3);
	animatedSprites->PlayAnimation("default", -1, 0.3f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;
	angle = 360;
	scaleX = 3;
	scaleY = 3;
	timer = 0;
	shotInterval = 0;
	attackTimer = 1;
	return true;
}

void Bob::Update(const double dElapsedTime)
{
	timer += dElapsedTime;

	if (!bIsActive)
		return;

	if (health <= 0)
		bIsActive = false;

	switch (sCurrentFSM)
	{
		case CEnemy2D::IDLE:
		{
			directionChosen = false;
			vec2Direction = glm::vec2(0, 0);
			if (iFSMCounter > iMaxFSMCounter)
			{
				sCurrentFSM = CHASE;
				iFSMCounter = 0;
				break;
			}
		}
		case CEnemy2D::CHASE:
		{
			directionChosen = false;
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 25.0f)
			{
				auto path = CEnemy2D::cMap2D->PathFind(vec2Index,
					cPlayer2D->vec2Index, heuristic::euclidean, 10);

				bool bFirstPosition = true;
				for (const auto& coord : path)
				{
					if (bFirstPosition == true)
					{
						vec2Destination = coord;

						vec2Direction = vec2Destination - vec2Index;
						vec2Direction = -vec2Direction;
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
			}
			else
			{
				sCurrentFSM = BOSSPHASE1;
				iFSMCounter = 0;
				break;
			}
			iFSMCounter++;
			break;
		}
		case CEnemy2D::BOSSPHASE1:
		{
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) <= 25.0f)
			{
				randomDirection();
				UpdatePosition();
				if (iFSMCounter > iMaxFSMCounter)
				{
					sCurrentFSM = IDLE;
					iFSMCounter = 0;
					break;
				}
			}
			else
			{
				sCurrentFSM = CHASE;
				iFSMCounter = 0;
				break;
			}
			if (shotInterval < 5)
			{
				for (int i = 0; i < 7; i += dElapsedTime)
				{

				}
			}
			iFSMCounter++;
			break;
		}
		case CEnemy2D::BOSSPHASE2:
		{
			auto path = CEnemy2D::cMap2D->PathFind(vec2Index,
				cPlayer2D->vec2Index, heuristic::euclidean, 10);

			bool bFirstPosition = true;
			for (const auto& coord : path)
			{
				if (bFirstPosition == true)
				{
					vec2Destination = coord;

					vec2Direction = vec2Destination - vec2Index;
					vec2Direction = -vec2Direction;
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

			if (iFSMCounter > iMaxFSMCounter)
			{
				sCurrentFSM = IDLE;
				iFSMCounter = 0;
				break;
			}
			iFSMCounter++;
			break;
		}
	}
	animatedSprites->Update(dElapsedTime);
	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

void Bob::UpdatePosition(void)
{
	// Store the old position
	vec2OldIndex = vec2Index;
	if (vec2Direction.y < 0)
	{
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y-= speed_multiplier;
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
			vec2Index.y = vec2OldIndex.y;
			vec2NumMicroSteps.y = 0;
		}
	}
	else if (vec2Direction.y > 0)
	{
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y+= speed_multiplier;

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
			vec2NumMicroSteps.y = 0;
		}
	}
	if (vec2Direction.x < 0)
	{
		// Move left
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x >= 0)
		{
			vec2NumMicroSteps.x-= speed_multiplier;
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
			vec2Index.x = vec2OldIndex.x;
			vec2NumMicroSteps.x = 0;
		}
	}
	else if (vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			vec2NumMicroSteps.x+= speed_multiplier;

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
	}
}

bool Bob::randomDirection()
{
	if (!directionChosen)
	{
		int i = rand() % 4;
		switch (i)
		{
		case 0:
		{
			if (CheckPosition(DOWN))
			{
				vec2Direction = glm::vec2(0, -1);
				directionChosen = true;
				return true;
			}
		}
		case 1:
		{
			if (CheckPosition(UP))
			{
				vec2Direction = glm::vec2(0, 1);
				directionChosen = true;
				return true;
			}
		}
		case 2:
		{
			if (CheckPosition(LEFT))
			{
				vec2Direction = glm::vec2(-1, 0);
				directionChosen = true;
				return true;
			}
		}
		case 3:
		{
			if (CheckPosition(RIGHT))
			{
				vec2Direction = glm::vec2(1, 0);
				directionChosen = true;
				return true;
			}
		}
		}
	}
	return false;
}

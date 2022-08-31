#include "Octopus.h"

Octopus::Octopus()
{
	maxHealth = 20;
	health = maxHealth;
	atk = 10;
}

Octopus::Octopus(glm::vec2 pos , int hpMulti)
{
	vec2Index = pos;
	maxHealth = 20 * hpMulti;
	health = maxHealth;
	atk = 10;
}

Octopus::~Octopus()
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

bool Octopus::Init(void)
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

	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/Enemies/octopus.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load enemy tile texture" << std::endl;
		return false;
	}

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	cSoundController = CSoundController::GetInstance();
	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;
	angle = 360;
	timer = 0;
	stuck = false;
	status = AILMENT::NONE;

	return true;
}

void Octopus::Update(const double dElapsedTime)
{
	timer += dElapsedTime;

	if (!bIsActive)
		return;

	if (health <= 0)
	{
		bIsActive = false;
		// spawn loot
		int i = rand() % 5;
		if (i == 3)
		{
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 10);
		}
		else if (i > 3)
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 51);
		// add kill counter
		cPlayer2D->addOctopusKillCount(1);
	}
	switch (sCurrentFSM)
	{
		case CEnemy2D::IDLE:
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				if (timer >= 1) // after 3s idle, go into patrol
				{
					timer = 0;
					sCurrentFSM = CHASE;
					iFSMCounter = 0;
				}
			}
			iFSMCounter++;
			break;
		}
		case CEnemy2D::CHASE:
		{
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 1.0f)
			{
				if (iFSMCounter > iMaxFSMCounter)
				{
					timer = 0;
					sCurrentFSM = ATTACK;
					iFSMCounter = 0;
					break;
				}
			}
			auto path = CEnemy2D::cMap2D->PathFind(vec2Index,
				cPlayer2D->vec2Index, heuristic::euclidean, 10);

			bool bFirstPosition = true;
			for (const auto& coord : path)
			{
				if (bFirstPosition == true)
				{
					vec2Destination = coord;

					if (!stuck)
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
			if (stuck)
			{
				if (CheckPosition(stuckDirection))
					stuck = false;
			}
			UpdatePosition();
			
			iFSMCounter++;
			break;
		}
		case CEnemy2D::ATTACK:
		{
			static float attackTimer = 0;
			attackTimer += dElapsedTime;
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) <= 1.0f)
			{
				if (attackTimer >= 1)
				{
					cSoundController->PlaySoundByID_2(13);
					cPlayer2D->LoseHealth(atk);
					attackTimer = 0;
				}
			}
			else
			{
				sCurrentFSM = IDLE;
				iFSMCounter = 0;
				timer = 0;
				break;
			}
			iFSMCounter++;
			break;
		}
	}
	UpdateStatus(dElapsedTime);
	//animatedSprites->Update(dElapsedTime);
	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

void Octopus::UpdatePosition(void)
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

		DIRECTION repositionDirection = LEFT;
		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(DOWN) == false)
		{
			vec2Index.y = vec2OldIndex.y;
			vec2NumMicroSteps.y = 0;
			if (!stuck)
				stuckPosition = vec2Index;
			if (vec2Direction.y < 0)
			{
				stuck = true;
				stuckDirection = DOWN;
			}
			if (stuck)
			{
				if (vec2Direction.x < 0 && CheckPosition(LEFT))
				{
					repositionDirection = LEFT;
				}
				else if (vec2Direction.x > 0 && CheckPosition(RIGHT))
				{
					repositionDirection = RIGHT;
				}
				if (repositionDirection == RIGHT || (vec2Index == stuckPosition && cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 1.0f))
					vec2Direction = glm::vec2(1, 0);
				else if (vec2Index != stuckPosition)
					vec2Direction = glm::vec2(-1, 0);
			}
		}

		// Interact with the Player
		//InteractWithPlayer();
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

		DIRECTION repositionDirection = LEFT;
		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(UP) == false)
		{
			//vec2Index = vec2OldIndex;
			vec2NumMicroSteps.y = 0;
			if (!stuck)
				stuckPosition = vec2Index;
			if (vec2Direction.y > 0)
			{
				stuck = true;
				stuckDirection = UP;
			}
			if (stuck)
			{
				if (vec2Direction.x < 0 && CheckPosition(LEFT))
				{
					repositionDirection = LEFT;
				}
				else if (vec2Direction.x > 0 && CheckPosition(RIGHT))
				{
					repositionDirection = RIGHT;
				}
				if (repositionDirection == RIGHT || (vec2Index == stuckPosition && cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 1.0f))
					vec2Direction = glm::vec2(1, 0);
				else if (vec2Index != stuckPosition)
					vec2Direction = glm::vec2(-1, 0);
			}
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
			vec2NumMicroSteps.x-= speed_multiplier;
			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(LEFT);

		DIRECTION repositionDirection = DOWN;
		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(LEFT) == false)
		{
			vec2Index.x = vec2OldIndex.x;
			vec2NumMicroSteps.x = 0;
			if (vec2Direction.x < 0)
			{
				stuck = true;
				stuckDirection = LEFT;
			}
			if (stuck)
			{
				if (vec2Direction.y < 0 && CheckPosition(DOWN))
				{
					repositionDirection = DOWN;
				}
				else if (vec2Direction.y > 0 && CheckPosition(UP))
				{
					repositionDirection = UP;
				}
				if (repositionDirection == UP)
					vec2Direction = glm::vec2(0, 1);
				else
					vec2Direction = glm::vec2(0, -1);
			}
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
			vec2NumMicroSteps.x+= speed_multiplier;

			if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				vec2NumMicroSteps.x = 0;
				vec2Index.x++;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(RIGHT);

		DIRECTION repositionDirection = DOWN;
		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(RIGHT) == false)
		{
			//vec2Index = vec2OldIndex;
			vec2NumMicroSteps.x = 0;
			if (vec2Direction.x > 0)
			{
				stuck = true;
				stuckDirection = RIGHT;
			}
			if (stuck)
			{
				if (vec2Direction.y < 0 && CheckPosition(DOWN))
				{
					repositionDirection = DOWN;
				}
				else if (vec2Direction.y > 0 && CheckPosition(UP))
				{
					repositionDirection = UP;
				}
				if (repositionDirection == UP)
					vec2Direction = glm::vec2(0, 1);
				else
					vec2Direction = glm::vec2(0, -1);
			}
		}
		// Interact with the Player
		//InteractWithPlayer();
	}
}

glm::vec2 Octopus::getVec2Index()
{
	return vec2Index;
}

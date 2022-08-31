#include "Spider.h"
#include "EventController.h"
Spider::Spider()
{
	maxHealth = 30;
	health = maxHealth;
	atk = 20;
}

Spider::Spider(glm::vec2 pos,int hpMulti)
{
	vec2Index = pos;
	maxHealth = 30 * hpMulti;
	health = maxHealth;
	atk = 20;
}

Spider::~Spider()
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

bool Spider::Init(void)
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

	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/Enemies/spider.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load Spider tile texture" << std::endl;
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
	directionChosen = false;
	shotInterval = 0;

	status = AILMENT::NONE;
	return true;
}

void Spider::Update(const double dElapsedTime)
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
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 11);
		}
		else if (i > 3)
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 21);
		// add kill counter
		cPlayer2D->addSpiderKillCount(1);
	}

	switch (sCurrentFSM)
	{
		case CEnemy2D::IDLE:
		{
			directionChosen = false;
			vec2Direction = glm::vec2(0, 0);
			if (iFSMCounter > iMaxFSMCounter)
			{
				if (timer >= 2.5) // after 3s idle, go into patrol
				{
					timer = 0;
					sCurrentFSM = PATROL;
					iFSMCounter = 0;
				}
			}
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 2.5f)
			{
				sCurrentFSM = RUN;
				iFSMCounter = 0;
			}
			else if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 2.5f && cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 5.0f)
			{
				sCurrentFSM = ATTACK;
				iFSMCounter = 0;
			}
			iFSMCounter++;
			break;
		}
		case CEnemy2D::PATROL:
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				timer = 0;
				sCurrentFSM = IDLE;
				iFSMCounter = 0;
				//cout << "Switching to Idle State" << endl;
			}
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 2.5f)
			{
				sCurrentFSM = RUN;
				iFSMCounter = 0;
			}
			else if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 2.5f && cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 5.0f)
			{
				sCurrentFSM = ATTACK;
				iFSMCounter = 0;
			}
			else
			{
				// Patrol around
				// Update the Enemy2D's position for patrol
				randomDirection();
				UpdatePosition();
			}
			iFSMCounter++;
			break;
		}
		case CEnemy2D::RUN:
		{
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 2.5f)
			{
				if (iFSMCounter > iMaxFSMCounter)
				{
					timer = 0;
					sCurrentFSM = IDLE;
					iFSMCounter = 0;
					break;
				}
			}

			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 2.5f)
			{
				vec2Direction = vec2Index - cPlayer2D->vec2Index;
				vec2Direction = -vec2Direction;

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
			}
			UpdatePosition();
			iFSMCounter++;
			break;
		}
		case CEnemy2D::ATTACK:
		{
			vec2Direction = glm::vec2(0, 0);
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 2.5f)
			{
				sCurrentFSM = RUN;
				iFSMCounter = 0;
				break;
			}
			else if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 5.0f)
			{
				sCurrentFSM = IDLE;
				iFSMCounter = 0;
				break;
			}
			
			shotInterval -= dElapsedTime;
			if (shotInterval <= 0)
			{
				cSoundController->PlaySoundByID_2(13);
				Webshot* webShot = new Webshot();
				webShot->SetShader("Shader2D_Colour");
				if (webShot->Init())
				{
					webShot->setDirection(cPlayer2D->getPreciseVec2Index(true) - getPreciseVec2Index(true));
					EventController::GetInstance()->spawnProjectiles(webShot, getPreciseVec2Index(true));
				}
				shotInterval = 5;
			}
		}
	}
	/*animatedSprites->Update(dElapsedTime);*/
	// Update the UV Coordinates
	UpdateStatus(dElapsedTime);
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

void Spider::UpdatePosition(void)
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
			directionChosen = false;
			randomDirection();
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

		DIRECTION repositionDirection = LEFT;
		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(UP) == false)
		{
			//vec2Index = vec2OldIndex;
			vec2NumMicroSteps.y = 0;
			directionChosen = false;
			randomDirection();
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

		DIRECTION repositionDirection = DOWN;
		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(LEFT) == false)
		{
			vec2Index.x = vec2OldIndex.x;
			vec2NumMicroSteps.x = 0;
			directionChosen = false;
			randomDirection();
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

		DIRECTION repositionDirection = DOWN;
		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(RIGHT) == false)
		{
			//vec2Index = vec2OldIndex;
			vec2NumMicroSteps.x = 0;
			directionChosen = false;
			randomDirection();
		}
	}
}

bool Spider::randomDirection()
{
	if (!directionChosen)
	{
		int i = rand() % 4;
		switch (i)
		{
		case 0:
		{
			if (CheckPosition(DOWN) && checkDirection(DOWN))
			{
				vec2Direction = glm::vec2(0, -1);
				directionChosen = true;
				return true;
			}
		}
		case 1:
		{
			if (CheckPosition(UP) && checkDirection(UP))
			{
				vec2Direction = glm::vec2(0, 1);
				directionChosen = true;
				return true;
			}
		}
		case 2:
		{
			if (CheckPosition(LEFT) && checkDirection(LEFT))
			{
				vec2Direction = glm::vec2(-1, 0);
				directionChosen = true;
				return true;
			}
		}
		case 3:
		{
			if (CheckPosition(RIGHT) && checkDirection(RIGHT))
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


glm::vec2 Spider::getVec2Index()
{
	return vec2Index;
}
bool Spider::checkDirection(DIRECTION eDirection)
{
	if (sCurrentFSM == RUN)
	{
		switch (eDirection)
		{
		case DOWN:
			if (vec2Index.y - cPlayer2D->vec2Index.y > 0)
				return false;
			break;
		case UP:
			if (vec2Index.y - cPlayer2D->vec2Index.y < 0)
				return false;
			break;
		case LEFT:
			if (vec2Index.x - cPlayer2D->vec2Index.x < 0)
				return false;
			break;
		case RIGHT:
			if (vec2Index.x - cPlayer2D->vec2Index.x > 0)
				return false;
			break;
		}
	}
	return true;
}

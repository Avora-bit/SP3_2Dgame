#include "Skeleton.h"

#include "EventController.h"

Skeleton::Skeleton()
{
	maxHealth = 100;
	health = maxHealth;
	atk = 10;
}

Skeleton::Skeleton(glm::vec2 pos, int hpMulti)
{
	vec2Index = pos;
	maxHealth = 100 * hpMulti;
	health = maxHealth;
	atk = 10;
}

Skeleton::~Skeleton()
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

bool Skeleton::Init(void)
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

	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/Enemies/Skull.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load Skeleton tile texture" << std::endl;
		return false;
	}

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(3, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("closed", 0, 0);
	animatedSprites->AddAnimation("opened", 1, 1);
	animatedSprites->AddAnimation("animatedMouth", 0, 1);
	animatedSprites->AddAnimation("sleep", 2, 2);
	animatedSprites->PlayAnimation("sleep", -1, 0.3f);

	cSoundController = CSoundController::GetInstance();
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

	status = AILMENT::NONE;
	sleep = true;
	sCurrentFSM = SLEEP;
	return true;
}

void Skeleton::Update(const double dElapsedTime)
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
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 12);
		}
		else if (i > 3)
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 22);
		// add kill counter
		cPlayer2D->addSkeletonKillCount(1);
	}

	if (sleep)
	{
		scaleX = 1;
		scaleY = 1;
	}
	else
	{
		scaleX = 3;
		scaleY = 3;
	}

	switch (sCurrentFSM)
	{
		case CEnemy2D::IDLE:
		{
			animatedSprites->PlayAnimation("closed", -1, 0.3f);
			vec2Direction = glm::vec2(0, 0);
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 10.0f + scaleX)
			{
				sCurrentFSM = CHASE;
				iFSMCounter = 0;
			}
			iFSMCounter++;
			break;
		}
		case CEnemy2D::SLEEP:
		{
			animatedSprites->PlayAnimation("sleep", -1, 0.3f);
			vec2Direction = glm::vec2(0, 0);
			if (!sleep)
			{
				sCurrentFSM = IDLE;
				iFSMCounter = 0;
			}
			iFSMCounter++;
			break;
		}
		case CEnemy2D::CHASE:
		{
			if (shotInterval < 1.5)
				animatedSprites->PlayAnimation("opened", -1, 0.3f);
			else 
				animatedSprites->PlayAnimation("closed", -1, 0.3f);
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 10.0f + scaleX && cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 2.5f + scaleX)
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
			else if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) >= 10.0f + scaleX)
			{
				sCurrentFSM = IDLE;
				iFSMCounter = 0;
			}
			shotInterval -= dElapsedTime;
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) <= -0.5f + scaleX)
			{
				sCurrentFSM = ATTACK;
				iFSMCounter = 0;
			}
			else if (shotInterval <= 0)
			{
				cSoundController->PlaySoundByID_2(14);
				SkeletonShot* skeletonShot = new SkeletonShot();
				skeletonShot->SetShader("Shader2D_Colour");
				if (skeletonShot->Init())
				{
					skeletonShot->setDirection(cPlayer2D->getPreciseVec2Index(true) - getPreciseVec2Index(true));
					EventController::GetInstance()->spawnProjectiles(skeletonShot, getPreciseVec2Index(true));
				}
				shotInterval = 5;
			}
			UpdatePosition();
			iFSMCounter++;
			break;
		}
		case CEnemy2D::ATTACK:
		{
			animatedSprites->PlayAnimation("animatedMouth", -1, 0.5f);
			attackTimer += dElapsedTime;
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) <= -0.5f + scaleX)
			{
				if (attackTimer >= 1.5)
				{
					cSoundController->PlaySoundByID_2(13);
					InteractWithPlayer();
					attackTimer = 0;
				}
			}
			else
			{
				sCurrentFSM = CHASE;
				iFSMCounter = 0;
				break;
			}
			iFSMCounter++;
			break;
		}
	}
	UpdateStatus(dElapsedTime);
	animatedSprites->Update(dElapsedTime);
	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

void Skeleton::UpdatePosition(void)
{
	// Store the old position
	vec2OldIndex = vec2Index;
	if (vec2Direction.y < 0)
	{
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
	}
	else if (vec2Direction.y > 0)
	{
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
	}
	if (vec2Direction.x < 0)
	{
		// Move left
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
	}
	else if (vec2Direction.x > 0)
	{
		// Move right
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
	}
}


glm::vec2 Skeleton::getVec2Index()
{
	return vec2Index;
}

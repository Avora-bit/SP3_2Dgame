#include "Skeleton.h"

Skeleton::Skeleton()
{
	health = 100;
	atk = 0;
}

Skeleton::Skeleton(glm::vec2 pos)
{
	vec2Index = pos;
	health = 100;
	atk = 0;
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

	eventController = EventController::GetInstance();

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

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(2, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("closed", 1, 1);
	animatedSprites->AddAnimation("opened", 2, 2);
	animatedSprites->PlayAnimation("closed", -1, 0.3f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;
	angle = 360;
	timer = 0;
	shotInterval = 0;
	attackTimer = 1;
	return true;
}

void Skeleton::Update(const double dElapsedTime)
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
			animatedSprites->PlayAnimation("closed", -1, 0.3f);
			vec2Direction = glm::vec2(0, 0);
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 10.0f)
			{
				sCurrentFSM = CHASE;
				iFSMCounter = 0;
			}
			iFSMCounter++;
			break;
		}
		case CEnemy2D::CHASE:
		{
			animatedSprites->PlayAnimation("closed", -1, 0.3f);
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 10.0f && cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) > 2.5f)
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
			else if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) >= 10.0f)
			{
				sCurrentFSM = IDLE;
				iFSMCounter = 0;
			}
			shotInterval -= dElapsedTime;
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) <= 1.0f)
			{
				sCurrentFSM = ATTACK;
				iFSMCounter = 0;
			}
			else if (shotInterval <= 0)
			{
				animatedSprites->PlayAnimation("opened", -1, 0.3f);
				SkeletonShot* skeletonShot = new SkeletonShot();
				skeletonShot->SetShader("Shader2D_Colour");
				if (skeletonShot->Init())
				{
					eventController->spawnProjectiles(skeletonShot, vec2Index);
				}
				shotInterval = 5;
			}
			UpdatePosition();
			iFSMCounter++;
			break;
		}
		case CEnemy2D::ATTACK:
		{
			animatedSprites->PlayAnimation("opened", -1, 0.3f);
			attackTimer += dElapsedTime;
			if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) <= 1.0f)
			{
				if (attackTimer >= 1)
				{
					cPlayer2D->LoseHealth(atk);
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
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y--;
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
			vec2NumMicroSteps.y++;

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
			vec2NumMicroSteps.x--;
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
			vec2NumMicroSteps.x++;

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
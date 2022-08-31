/**
 Shivs2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Projectile2D.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CProjectile2D::CProjectile2D(void)
	: cMap2D(NULL)
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
CProjectile2D::~CProjectile2D(void)
{
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
bool CProjectile2D::Init(void)
{
	// Get the handler to the CSettings instance

	camera = Camera::GetInstance();

	cSettings = CSettings::GetInstance();

	cPlayer2D = CPlayer2D::GetInstance();
	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to CShivs2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;

	// Set the start position of the Player to iRow and iCol
	vec2Index = cPlayer2D->vec2Index;
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load the ground texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/SP3Images/Weapons/Shiv.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load Shiv tile texture" << std::endl;
		return false;
	}
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("active", 0, 0);
	animatedSprites->PlayAnimation("active", -1, 1.f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	cSoundController = CSoundController::GetInstance();

	bIsActive = true;
	scaleX = 1;
	scaleY = 1;
	return true;
}

/**
 @brief Update this instance
 */
void CProjectile2D::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;
	trajectory();

	InteractWithMap();
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CProjectile2D::PreRender(void)
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
void CProjectile2D::Render(void)
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
	float angle = (atan2(vec2Direction.y, vec2Direction.x) / 3.14159265359) * 180.0;
	transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));

	transform = glm::scale(transform, glm::vec3(scaleX, scaleY, 1));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(runtimeColour));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	animatedSprites->Render();

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CProjectile2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

void CProjectile2D::setDirection(glm::vec2 direction)
{
	vec2Direction = direction;
}

void CProjectile2D::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (vec2Index.x < 0)
		{
			vec2Index.x = 0;
			vec2NumMicroSteps.x = 0;
			bIsActive = false;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			vec2NumMicroSteps.x = 0;
			bIsActive = false;
		}
	}
	else if (eDirection == UP)
	{
		if (vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			vec2NumMicroSteps.y = 0;
			bIsActive = false;
		}
	}
	else if (eDirection == DOWN)
	{
		if (vec2Index.y < 0)
		{
			vec2Index.y = 0;
			vec2NumMicroSteps.y = 0;
			bIsActive = false;
		}
	}
	else
	{
		cout << "Projectiles2D::Constraint: Unknown direction." << endl;
	}
}

void CProjectile2D::InteractWithMap(void)
{
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x))
	{
	case 101:
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		vec2NumMicroSteps.x = 0;

		distanceTravelled = 0;
		break;
	}
}

bool CProjectile2D::CheckPosition(DIRECTION eDirection)
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

	return true;
}

void CProjectile2D::trajectory()			//update position
{
				//vertical movement
	const int iOldIndexY = vec2Index.y;

	//glm::vec2 trajectoryDir = vec2Direction;
	glm::vec2 trajectoryDir = glm::normalize(vec2Direction);

	vec2NumMicroSteps.y += trajectoryDir.y;
	if (vec2NumMicroSteps.y < 0)
	{
		vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
		vec2Index.y--;

		Constraint(DOWN);
	}
	else if (vec2NumMicroSteps.y >= cSettings->NUM_STEPS_PER_TILE_YAXIS)
	{
		vec2NumMicroSteps.y = 0;
		vec2Index.y++;

		Constraint(UP);
	}

	// Find a feasible position for the enemy2D's current position
	// Constraint the enemy2D's position within the screen boundary
	
			//horizontal movement
	const int iOldIndexX = vec2Index.x;

	vec2NumMicroSteps.x += trajectoryDir.x;
	if (vec2NumMicroSteps.x < 0)
	{
		vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
		vec2Index.x--;

		Constraint(LEFT);
	}
	else if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
	{
		vec2NumMicroSteps.x = 0;
		vec2Index.x++;

		Constraint(RIGHT);
	}
	// Find a feasible position for the enemy2D's current position
	if (!CheckPosition(LEFT) || !CheckPosition(RIGHT) || !CheckPosition(UP) || !CheckPosition(DOWN)) 
		bIsActive = false;
	// Constraint the enemy2D's position within the screen boundary
	
	// Interact with the Player
	InteractWithPlayer();
}

bool CProjectile2D::InteractWithPlayer()
{
	return false;
}
/**
 Shivs2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "BobShot.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
BobShot::BobShot()
{
	atk = 10;
}

/**
// @brief Destructor This destructor has protected access modifier as this class will be a Singleton
// */
BobShot::~BobShot(void)
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
bool BobShot::Init(void)
{
	// Get the handler to the CSettings instance
	camera = Camera::GetInstance();

	cSettings = CSettings::GetInstance();

	cPlayer2D = CPlayer2D::GetInstance();
	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	distanceTravelled = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Load the ground texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/SP3Images/Enemies/Attacks/fireball.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load BobShot tile texture" << std::endl;
		return false;
	}
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, cSettings->TILE_WIDTH/2, cSettings->TILE_HEIGHT/2);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	cSoundController = CSoundController::GetInstance();

	bIsActive = true;
	scaleX = 2;
	scaleY = scaleX;
	return true;
}

void BobShot::setDirection(glm::vec2 direction)
{
	vec2Direction = direction;
}

/**
 @brief Update this instance
 */
void BobShot::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;

	trajectory();

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

bool BobShot::InteractWithPlayer()
{
	glm::vec2 vec2PlayerPos = cPlayer2D->vec2Index;

	// Check if the enemy2D is within 1.5 indices of the player2D
	if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->getPreciseVec2Index(true)) <= -0.5f + scaleX)
	{
		bIsActive = false;
		cPlayer2D->LoseHealth(atk);
		return true;
	}
	return false;
}

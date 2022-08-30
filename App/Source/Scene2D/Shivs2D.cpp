/**
 Shivs2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Shivs2D.h"

 /**
  @brief Constructor This constructor has protected access modifier as this class will be a Singleton
  */
CShivs2D::CShivs2D(void)
{
	atk = 10;
}

/**
// @brief Destructor This destructor has protected access modifier as this class will be a Singleton
// */
CShivs2D::~CShivs2D(void)
{
	cMap2D = NULL;
	camera = NULL;

	if (cSoundController) cSoundController = NULL;

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
bool CShivs2D::Init(void)
{
	camera = Camera::GetInstance();
	cSettings = CSettings::GetInstance();
	cPlayer2D = CPlayer2D::GetInstance();
	cMap2D = CMap2D::GetInstance();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cSoundController = CSoundController::GetInstance();

	// Load the ground texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/SP3Images/Weapons/shiv.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load CShivs2D tile texture" << std::endl;
		return false;
	}
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	bIsActive = true;

	cPhysics2D.Init();
	//vec2Direction = cPlayer2D->vec2Index - vec2Index;

	return true;
}

/**
 @brief Update this instance
 */
void CShivs2D::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;
	trajectory();

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

bool CShivs2D::InteractWithEnemy(CEnemy2D* enemy)
{
	glm::vec2 vec2EnemyPos = enemy->vec2Index;
	if (((vec2Index.x >= vec2EnemyPos.x - 0.5) &&
		(vec2Index.x <= vec2EnemyPos.x + 0.5))
		&&
		((vec2Index.y >= vec2EnemyPos.y - 0.5) &&
			(vec2Index.y <= vec2EnemyPos.y + 0.5)))
	{
		bIsActive = false;
		enemy->takeDamage(atk);
		return true;
	}
	return false;
}

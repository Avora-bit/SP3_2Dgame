/**
 Shivs2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Webshot.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
//Webshot::Webshot(void)
//{
//
//}
//
///**
// @brief Destructor This destructor has protected access modifier as this class will be a Singleton
// */
//Webshot::~Webshot(void)
//{
//
//}

/**
  @brief Initialise this instance
  */
bool Webshot::Init(void)
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

	distanceTravelled = 0;

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

	// Load the player texture
	// Load the ground texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/SP3Images/Weapons/Shiv.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Failed to load Shiv tile texture" << std::endl;
		return false;
	}
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 2, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("hold", 0, 0);
	animatedSprites->AddAnimation("throw", 1, 1);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	cSoundController = CSoundController::GetInstance();

	return true;
}

/**
 @brief Update this instance
 */
void Webshot::Update(const double dElapsedTime)
{
	vec2OldIndex = vec2Index;
	if (!cPlayer2D->getProjectileForce())
	{
		if (cPlayer2D->getProjectileForce() >= 1.5)
		{
			cPhysics2D.SetTime((float)dElapsedTime);
			cPhysics2D.SetInitialVelocity(glm::vec2(cPlayer2D->getProjectileForce(), 0.0f));
			cPhysics2D.Update();
			glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();
			int iIndex_XAxis_OLD = vec2Index.x;
			int iDisplacement_MicroSteps = (int)(v2Displacement.x / cSettings->MICRO_STEP_XAXIS);
			switch (cPlayer2D->getAttackDirection())
			{
			/*case LEFT:
				if (vec2Index.x >= 0)
				{
					vec2NumMicroSteps.x -= fabs(iDisplacement_MicroSteps);
					if (vec2NumMicroSteps.x < 0)
					{
						vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
						vec2Index.x--;
						distanceTravelled++;
					}
				}

				if (!CheckPosition(LEFT) || !Constraint(LEFT) || distanceTravelled >= cPlayer2D->getBowForce() + 1)
				{
					vec2Index = vec2OldIndex;
					vec2NumMicroSteps.x = 0;

					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
					thrown = true;
					distanceTravelled = 0;
				}
				break;
			case RIGHT:
				if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
				{
					vec2NumMicroSteps.x += fabs(iDisplacement_MicroSteps);
					if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
					{
						vec2NumMicroSteps.x = 0;
						vec2Index.x++;
						distanceTravelled++;
					}
				}

				if (!CheckPosition(RIGHT) || !Constraint(RIGHT) || distanceTravelled >= cPlayer2D->getBowForce() + 1)
				{
					if (Constraint(RIGHT))
						vec2Index = vec2OldIndex;
					vec2NumMicroSteps.x = 0;

					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
					thrown = true;
					distanceTravelled = 0;
				}
				break;*/
			}
		}
		else
		{
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
			int iIndex_YAxis_Proposed = vec2Index.y;
			for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
			{
				vec2Index.y = i;
				/*if (CheckPosition(DOWN) == false)
				{
					if (i != iIndex_YAxis_OLD)
						vec2Index.y = i + 1;
					cPhysics2D.SetStatus(CPhysics2D::STATUS::FLY);
					vec2NumMicroSteps.y = 0;
					break;
				}*/
			}
		}
	}
	else
	{
		vec2Index = cPlayer2D->vec2Index;
	}

	InteractWithMap();
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}
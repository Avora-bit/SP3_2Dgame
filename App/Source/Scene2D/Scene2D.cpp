/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Scene2D.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
#include "Sword2D.h"

#include "System\filesystem.h"




/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void)
	:cMap2D(NULL)
	, cPlayer2D(NULL)
	, CShivs2D(NULL)
	, cGUI_Scene2D(NULL)
	, camera(NULL)
	, cKeyboardController(NULL)
	, cMouseController(NULL)
	, cGameManager(NULL)
	, cSoundController(NULL)
{
}

/**
 @brief Destructor
 */
CScene2D::~CScene2D(void)
{
	if (cKeyboardController)
	{
		// We won't delete this since it was created elsewhere
		cKeyboardController = NULL;
	}

	if (cSoundController)
	{
		cSoundController->StopAllSound();
		cSoundController = NULL;
	}

	if (camera)
	{
		camera = NULL;
	}

	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}

	if (cPlayer2D)
	{
		cPlayer2D->Destroy();
		cPlayer2D = NULL;
	}

	if (CShivs2D)
	{
		CShivs2D->Destroy();
		CShivs2D = NULL;
	}

	for (int i = 0; i < enemyVector.size(); i++)
	{
		delete enemyVector[i];
		enemyVector[i] = NULL;
	}
	enemyVector.clear();


	for (int i = 0; i < itemVector.size(); i++)
	{
		delete itemVector[i];
		itemVector[i] = NULL;
	}
	itemVector.clear();

	if (cGUI_Scene2D)
	{
		cGUI_Scene2D->Destroy();
		cGUI_Scene2D = NULL;
	}

	if (cGameManager)
	{
		cGameManager->Destroy();
		cGameManager = NULL;
	}

}

/**
@brief Init Initialise this instance
*/ 
bool CScene2D::Init( const unsigned int uiNumLevels,
					 const unsigned int uiNumRows,
					 const unsigned int uiNumCols)
{
	CShaderManager::GetInstance()->Use("Shader2D");

	cMap2D = CMap2D::GetInstance();

	cMap2D->SetShader("Shader2D");

	//generate island
	{
		MapGen* island = new MapGen;
		island->createMap(CSettings::GetInstance()->NUM_TILES_XAXIS,
						CSettings::GetInstance()->NUM_TILES_YAXIS);
		island->randomfill();
		for (int i = 0; i < 20; i++) {				//rounding out edges
			island->updateIsland();
		}
		island->growsand();		//sand radius of 1
		//replace proper keys
		island->convertKeys();
		island->randreplace(200, 98);			//replace sand with player

		int randspawn = rand() % 5 + 5;		//random number of cross, 5-10 cross
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(96, 98);			//replace sand with cross
		}

		randspawn = rand() % 20 + 20;		//random number of trees, 20-40 trees
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(96, 98);			//replace grass with tree
		}

		string filename = "Maps/Island.csv";
		island->exportmap(filename);
		//clean
		delete island;
		island = nullptr;
	}
	//generate dungeon
	{
		MapGen* dungeon = new MapGen;
		dungeon->createMap(CSettings::GetInstance()->NUM_TILES_XAXIS,
			CSettings::GetInstance()->NUM_TILES_YAXIS);
		dungeon->randomfill();
		for (int i = 0; i < 20; i++) {				//rounding out edges
			dungeon->updateMap();
		}
		dungeon->growsand();		//sand radius of 1
		//replace proper keys
		dungeon->convertKeys();
		dungeon->randreplace(200, 98);			//replace sand with player

		int randspawn = rand() % 5 + 5;		//random number of chests, 5-10 chests
		for (int i = 0; i < randspawn; i++) {
			dungeon->randreplace(96, 98);			//replace sand with cross
		}

		randspawn = rand() % 20 + 20;		//random number of chests, 20-40 trees
		for (int i = 0; i < randspawn; i++) {
			dungeon->randreplace(96, 98);			//replace grass with tree
		}

		string filename = "Maps/Dungeon.csv";
		dungeon->exportmap(filename);
		//clean
		delete dungeon;
		dungeon = nullptr;
	}


	if (cMap2D->Init(2, CSettings::GetInstance()->NUM_TILES_YAXIS,
						CSettings::GetInstance()->NUM_TILES_XAXIS) == false)
	{
		cout << "Failed to load CMap2D" << endl;
		return false;
	}

	if (cMap2D->LoadMap("Maps/Island.csv", 0) == false)
	{
		cout << "Failed to load Island.csv" << endl;
		return false;
	}
	if (cMap2D->LoadMap("Maps/Dungeon.csv", 1) == false)
	{
		cout << "Failed to load Dungeon.csv" << endl;
		return false;
	}

	/*if (cMap2D->LoadMap("Maps/100x100.csv", 1) == false)
	{
		return false;
	}*/
	
	CShaderManager::GetInstance()->Use("Shader2D_Colour");

	cPlayer2D = CPlayer2D::GetInstance();

	cPlayer2D->SetShader("Shader2D_Colour");

	if (!cPlayer2D->Init())
	{
		cout << "Failed to load CPlayer2D" << endl;
		return false;
	}

	CShivs2D = CShivs2D::GetInstance();

	CShivs2D->SetShader("Shader2D_Colour");

	if (!CShivs2D->Init())
	{
		cout << "Failed to load CShivs2D" << endl;
		return false;
	}

	camera = Camera::GetInstance();
	camera->Init();

	enemyVector.clear();
	/*
	while (true)
	{
		Octopus* octo = new Octopus();
		octo->SetShader("Shader2D_Colour");
		
		if (octo->Init())
		{
			octo->SetPlayer2D(cPlayer2D);
			enemyVector.push_back(octo);
		}
		else
			break;
	}
	while (true)
	{
		Chicken* chicken = new Chicken();
		chicken->SetShader("Shader2D_Colour");

		if (chicken->Init())
		{
			chicken->SetPlayer2D(cPlayer2D);
			enemyVector.push_back(chicken);
		}
		else
			break;
	}
	*/
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();

	if (!cGUI_Scene2D->Init())
	{
		cout << "Failed to load CGUI_Scene2D" << endl;
		return false;
	}

	cGameManager = CGameManager::GetInstance();
	cGameManager->Init();

	cKeyboardController = CKeyboardController::GetInstance();

	cSoundController = CSoundController::GetInstance();

	// BGM
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Adventure.ogg"), 1, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Romance in the Air.ogg"), 2, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Advance.ogg"), 3, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\The Bullet Bill Express.ogg"), 4, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sword Throw.ogg"), 5, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sword Throw.ogg"), 6, true);


	cSoundController->AddToPlaylist(3);
	cSoundController->AddToPlaylist(4);
	cSoundController->AddToPlaylist(2);
	cSoundController->AddToPlaylist(1);

	cPhysics2D.Init();

	soundVol = 1.f;

	/*for (int i = 1; i < 5; i++)
	{*/
		//cSoundController->setVolume(cSoundController->return_currentMusic(), musicsfx, musicVol);
	/*}*/
	//cSoundController->SetMasterVolume(0.01f);



	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{
	//setvo


	cPlayer2D->Update(dElapsedTime);

	if (CInventoryManager::GetInstance()->Check("Sword"))
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->Update(dElapsedTime);
	}
	
	CShivs2D->Update(dElapsedTime);

	cMap2D->Update(dElapsedTime);

	cSoundController->Update(dElapsedTime);

	//vec2Destination = cPlayer2D->vec2Index;
	//(enemy, player)
	/*float fDistance = cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->);

	if (cPlayer2D->getx())
	{

	}*/



	float trackingPosX = cPlayer2D->vec2Index.x + (cPlayer2D->vec2NumMicroSteps.x / CSettings::GetInstance()->NUM_STEPS_PER_TILE_XAXIS);
	float trackingPosY = cPlayer2D->vec2Index.y + (cPlayer2D->vec2NumMicroSteps.y / CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS);

	camera->Update(dElapsedTime, glm::vec2(trackingPosX, trackingPosY));

	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->Update(dElapsedTime);
	}

	cGUI_Scene2D->Update(dElapsedTime);

	static bool buttonPress = false;
	if (cKeyboardController->IsKeyDown(GLFW_KEY_F1) && !buttonPress)
	{
		buttonPress = true;
		if (cMap2D->GetCurrentLevel() != 0)
			cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() - 1);
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_F2) && !buttonPress)
	{
		buttonPress = true;
		if (cMap2D->GetCurrentLevel() != 3)
			cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 1);
	}
	else if (!(cKeyboardController->IsKeyDown(GLFW_KEY_F1)||cKeyboardController->IsKeyDown(GLFW_KEY_F2))&&buttonPress)
		buttonPress = false;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_F6))
	{
		try {
			if (!cMap2D->SaveMap("Maps/DM2213_Map_Level_01_SAVEGAMEtest.csv"))
			{
				throw runtime_error("Unable to save the current game to a file");
			}
		}
		catch (runtime_error e)
		{
			cout << "Runtime error: " << e.what();
			return false;
		}
		return true;
	}

	if (cGameManager->bLevelCompleted)
	{
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 1);
		cGameManager->bLevelCompleted = false;
	}
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CScene2D::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(37/256.f, 50/256.f, 101/256.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);
}

/**
 @brief Render Render this instance
 */
void CScene2D::Render(void)
{
	cMap2D->PreRender();
	cMap2D->Render();
	cMap2D->PostRender();

	cPlayer2D->PreRender();
	cPlayer2D->Render();
	cPlayer2D->PostRender();

	if (CInventoryManager::GetInstance()->Check("Sword"))
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->PreRender();
		sword->Render();
		sword->PostRender();
	}

	CShivs2D->PreRender();
	CShivs2D->Render();
	CShivs2D->PostRender();

	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->PreRender();
		enemyVector[i]->Render();
		enemyVector[i]->PostRender();
	}

	cGUI_Scene2D->PreRender();
	cGUI_Scene2D->Render();
	cGUI_Scene2D->PostRender();
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{
}

float CScene2D::returnmusicvol()
{
	return musicVol;
}

void CScene2D::setmusicvol(float vol)
{
	musicVol = vol;
}

float CScene2D::returnsoundvol()
{
	return soundVol;
}

void CScene2D::setsoundvol(float vol)
{
	soundVol = vol;
}


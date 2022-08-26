/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Scene2D.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"
#include "Sword2D.h"

#include "System\filesystem.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void)
	: cMap2D(NULL)
	, cPlayer2D(NULL)
	, cGUI_Scene2D(NULL)
	, camera(NULL)
	, cKeyboardController(NULL)
	, cMouseController(NULL)
	, cGameManager(NULL)
	, cSoundController(NULL)
	, eventcontroller(NULL)
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
		island->randomfill(2, 1);
		//stabilize map
		for (int i = 0; i < 20; i++) {				//rounding out edges
			island->updateIsland();
		}
		island->growtile(2);		//smooth edge
		island->growtile(3);		//grow sand
		//replace proper keys
		island->convertKeys();

		string BGfilename = "Maps/IslandBG.csv";
		island->exportmap(BGfilename);
		std::cout << "BG exported";
		//foreground
		//convert keys back into array
		island->revertKeys();
		//delete water
		island->deleteall(1);			//delete all sand
		//spawn player
		island->randreplace(200, 3);
		//for (int i = 0; i < 2; i++)
		//{
		//	island->randreplace(303, 3); //test enemies
		//}
		//populate the foreground
		//cross on sand
		int randspawn = rand() % 10 + 20;		//random number of cross, 20-30 cross
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(8, 3);			//replace sand with cross
		}
		island->deleteall(3);			//delete all sand


		//tree on grass
		randspawn = rand() % 100 + 400;			//400-500 trees
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(6, 2);			//replace grass with tree
		}


		randspawn = rand() % 50 + 100;			//50-100 stick
		for (int i = 0; i < randspawn; i++) {
			//randreplace(itemid, itemkey)
			island->randreplace(30, 2);			//replace grass with sticks
		}

		randspawn = rand() % 50 + 100;
		for (int i = 0; i < randspawn; i++) {
			//randreplace(itemid, itemkey)
			island->randreplace(40, 2);			//replace grass with wood
		}


		island->deleteall(2);			//delete all grass




		//{0, 0},				//void
		//{ 1, 97 },			//water		//cannot dash, slows movement speed
		//{ 2, 99 },			//grass		//spawn tree
		//{ 3, 98 },			//sand		//spawn cross
		//{ 4, 96 },			//brick floor		//no behavior
		//{ 5, 95 },			//trap				//deals small amount of damage to the player, 5 hp
		////solid tiles
		//{ 6, 100 },			//tree
		//{ 7, 101 },			//brick wall
		////foreground tiles
		//{ 8, 80 },			//cross		//spawn treasure
		//{ 9, 79 },			//treasure	//spawn loot
		//{ 10, 78 },			//ladderdown
		//{ 11, 77 },			//ladderup
		//{ 12, 76 },			//web		//slows player

		



		




		//spawn structure with ladderdown

		//randspawn = rand() % 20 + 20;		//random number of trees, 20-40 trees
		//for (int i = 0; i < randspawn; i++) {
		//	island->randreplace(96, 98);			//replace grass with tree
		//}

		island->convertKeys();
		string FGfilename = "Maps/IslandFG.csv";
		island->exportmap(FGfilename);
		std::cout << "FG exported";
		//clean
		delete island;
		island = nullptr;
	}
	//generate dungeon
	{
		//MapGen* dungeon = new MapGen;
		//dungeon->createMap(CSettings::GetInstance()->NUM_TILES_XAXIS,
		//	CSettings::GetInstance()->NUM_TILES_YAXIS);
		//dungeon->randomfill();
		////for (int i = 0; i < 20; i++) {				//rounding out edges
		////	dungeon->updateMap();
		////}
		//dungeon->growsand();		//sand radius of 1
		////replace proper keys
		//dungeon->convertKeys();
		//dungeon->randreplace(200, 98);			//replace sand with player

		//string BGfilename = "Maps/DungeonBG.csv";
		//dungeon->exportmap(BGfilename);

		////foreground
		//string FGfilename = "Maps/DungeonFG.csv";

		////ladder up

		////spike trap

		////treasure chest

		////spawn last room with boss

		//dungeon->exportmap(FGfilename);
		////clean
		//delete dungeon;
		//dungeon = nullptr;
	}


	if (cMap2D->Init(2, CSettings::GetInstance()->NUM_TILES_YAXIS,
						CSettings::GetInstance()->NUM_TILES_XAXIS) == false)
	{
		cout << "Failed to load CMap2D" << endl;
		return false;
	}

	if (cMap2D->LoadMap("Maps/IslandBG.csv", "Maps/IslandFG.csv", 0) == false)
	{
		cout << "Failed to load Island" << endl;
		return false;
	}
	/*if (cMap2D->LoadMap("Maps/DungeonBG.csv", "Maps/DungeonFG.csv", 1) == false)
	{
		cout << "Failed to load Dungeon.csv" << endl;
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
	camera = Camera::GetInstance();
	camera->Init();

	eventcontroller = EventController::GetInstance();
	eventcontroller->Init();
	while (true)
	{
		Octopus* octo = new Octopus(glm::vec2(50,50));
		octo->SetShader("Shader2D_Colour");

		if (octo->Init())
		{
			octo->SetPlayer2D(cPlayer2D);
			eventcontroller->spawnEnemies(octo);
			break;
		}
		else
			break;
	}
	while (true)
	{
		Chicken* chicken = new Chicken(cPlayer2D->vec2Index);
		chicken->SetShader("Shader2D_Colour");

		if (chicken->Init())
		{
			chicken->SetPlayer2D(cPlayer2D);
			eventcontroller->spawnEnemies(chicken);
			break;
		}
		else
			break;
	}
	while (true)
	{
		Spider* spider = new Spider(cPlayer2D->vec2Index);
		spider->SetShader("Shader2D_Colour");

		if (spider->Init())
		{
			spider->SetPlayer2D(cPlayer2D);
			eventcontroller->spawnEnemies(spider);
			break;
		}
		else
			break;
	}
	/*while (true)
	{
		Skeleton* skeleton = new Skeleton(cPlayer2D->vec2Index);
		skeleton->SetShader("Shader2D_Colour");

		if (skeleton->Init())
		{
			skeleton->SetPlayer2D(cPlayer2D);
			eventcontroller->spawnEnemies(skeleton);
			break;
		}
		else
			break;
	}*/
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
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Grab.ogg"), 6, true);

	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_GrassWalk.ogg"), 7, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_WaterWalk.ogg"), 8, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_SandWalk.ogg"), 9, true);


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

	cMap2D->Update(dElapsedTime);

	cSoundController->Update(dElapsedTime);

	//vec2Destination = cPlayer2D->vec2Index;
	//(enemy, player)
	/*float fDistance = cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->);*/

	float trackingPosX = cPlayer2D->vec2Index.x + (cPlayer2D->vec2NumMicroSteps.x / CSettings::GetInstance()->NUM_STEPS_PER_TILE_XAXIS);
	float trackingPosY = cPlayer2D->vec2Index.y + (cPlayer2D->vec2NumMicroSteps.y / CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS);

	camera->Update(dElapsedTime, glm::vec2(trackingPosX, trackingPosY));

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

	if (cGameManager->bLevelIncrease)
	{
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 1);
		cGameManager->bLevelIncrease = false;
	}
	if (cGameManager->bLevelDecrease && cMap2D->GetCurrentLevel() > 0)
	{
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() - 1);
		cGameManager->bLevelDecrease = false;
	}

	eventcontroller->update(dElapsedTime);

	return true;
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

	if (CInventoryManager::GetInstance()->Check("Sword") && cPlayer2D->getAttacking())
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->PreRender();
		sword->Render();
		sword->PostRender();
	}

	for (int i = 0; i < eventcontroller->enemyVector.size(); i++)
	{
		eventcontroller->enemyVector[i]->PreRender();
		eventcontroller->enemyVector[i]->Render();
		eventcontroller->enemyVector[i]->PostRender();
	}

	for (int i = 0; i < eventcontroller->projectileVector.size(); i++)
	{
		eventcontroller->projectileVector[i]->PreRender();
		eventcontroller->projectileVector[i]->Render();
		eventcontroller->projectileVector[i]->PostRender();
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


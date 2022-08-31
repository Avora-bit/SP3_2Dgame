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
	, soundsfx(NULL)
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


	if (soundsfx)
	{
		delete soundsfx;
		soundsfx = NULL;
	}

}



//FIND LADDER
//SPAWN PLAYER RIGHT NEXT TO THE LADDDER



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
		island->randomfill(MapGen::Grass, MapGen::Water);
		//stabilize map
		for (int i = 0; i < 20; i++) {				//rounding out edges, stabilize islands
			island->updateIsland();
		}
		island->growtile(MapGen::Grass);		//smooth edge
		island->growtile(MapGen::Sand);			//grow sand

		vector<int>structure_ruins = {		//5,5 structure
			99,99,99,96,101,96,101,101,96,
			96,99,96,96,101,99,99,101,99,
			96,99,101,96,101,99,99,101,99,
			96,101,101,96,96,99,101,101,101,
			96,101,96,96,78,96,96,96,101,
			99,101,96,96,96,96,101,96,101,
			99,96,96,96,96,96,101,96,101,
			101,96,101,96,96,101,96,96,96,
			96,101,101,101,101,96,96,96,96,
		};

		//place structure
		island->placeRuins(structure_ruins, 9, 9);

		string BGfilename = "Maps/IslandBG.csv";
		island->exportmap(BGfilename, 0);
		std::cout << "BG exported";
		//foreground
		//delete water
		island->deleteall(MapGen::Water);			//delete all water
		//spawn player
		island->randreplace(MapGen::Player, MapGen::Sand);
		//populate the foreground
		//cross on sand
		int randspawn = rand() % 10 + 20;		//random number of cross, 20-30 cross
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(MapGen::Cross, MapGen::Sand);			//replace sand with cross
		}
		island->deleteall(MapGen::Sand);			//delete all sand

		//tree on grass
		randspawn = rand() % 200 + 600;			//600-800 trees
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(MapGen::Tree, MapGen::Grass);			//replace grass with tree
		}

		randspawn = rand() % 50 + 100;			//50-150 stick
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(MapGen::Stick, MapGen::Grass);			//replace grass with sticks
		}

		randspawn = rand() % 50 + 100;			//50-150 wood
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(MapGen::Wood, MapGen::Grass);			//replace grass with wood
		}

		randspawn = rand() % 25 + 75;
		for (int i = 0; i < randspawn; i++) {
			island->randreplace(MapGen::Rock, MapGen::Grass);			//replace grass with rocks
		}

		island->deleteall(MapGen::Grass);			//delete all grass
		island->deleteall(MapGen::BrickFloor);			//delete all brickfloor

		//spawn structure with ladderdown

		string FGfilename = "Maps/IslandFG.csv";
		island->exportmap(FGfilename, 1);
		std::cout << "FG exported";
		//clean
		delete island;
		island = nullptr;
	}
	//generate dungeon
	{
		MapGen* dungeon = new MapGen;
		dungeon->createMap(CSettings::GetInstance()->NUM_TILES_XAXIS,
			CSettings::GetInstance()->NUM_TILES_YAXIS);

		dungeon->generateDungeon(20);			//max features

		//spike trap
		int randspawn = rand() % 100 + 100;			//100-200 trap
		for (int i = 0; i < randspawn; i++) {
			dungeon->randreplace(MapGen::Trap, MapGen::BrickFloor);			//replace brickfloor with trap
		}

		string BGfilename = "Maps/DungeonBG.csv";
		dungeon->exportmap(BGfilename, 0);
		std::cout << "BG exported";

		//foreground
		//treasure chest
		randspawn = rand() % 10 + 10;			//10-20 treasure
		for (int i = 0; i < randspawn; i++) {
			dungeon->randreplace(MapGen::Treasure, MapGen::BrickFloor);			//replace brickfloor with treasure
		}


		//delete background
		dungeon->deleteall(MapGen::BrickFloor);			//delete all brickfloor
		dungeon->deleteall(MapGen::Trap);				//delete all trap

		string FGfilename = "Maps/DungeonFG.csv";
		dungeon->exportmap(FGfilename, 1);
		std::cout << "FG exported";
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
	else if (cMap2D->LoadMap("Maps/IslandBG.csv", "Maps/IslandFG.csv") == false)
	{
		cout << "Failed to load Island.csv" << endl;
		return false;
	}
	else if (cMap2D->LoadMap("Maps/DungeonBG.csv", "Maps/DungeonFG.csv", 1) == false)
	{
		cout << "Failed to load Dungeon.csv" << endl;
		return false;
	}
	
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
	hpMulti = 1;
	int i = 0;
	while (i < 10) //spawn 10 chicken
	{
		if (availableRandomSpawn())
		{
			Chicken* chicken = new Chicken(glm::vec2(randX, randY));
			chicken->SetShader("Shader2D_Colour");

			if (chicken->Init())
			{
				chicken->SetPlayer2D(cPlayer2D);
				eventcontroller->spawnEnemies(chicken);
				i++;
			}
			else
				break;
		}
	}
	i = 0;
	while (i < 5)
	{
		if (availableRandomSpawn())
		{
			Spider* spider = new Spider(glm::vec2(randX, randY),hpMulti);
			spider->SetShader("Shader2D_Colour");

			if (spider->Init())
			{
				spider->SetPlayer2D(cPlayer2D);
				eventcontroller->spawnEnemies(spider);
i++;
			}
			else
			break;
		}
	}
	i = 0;
	while (i < 2)
	{
		if (availableRandomSpawn())
		{
			Skeleton* skeleton = new Skeleton(glm::vec2(randX, randY), hpMulti);
			skeleton->SetShader("Shader2D_Colour");

			if (skeleton->Init())
			{
				skeleton->SetPlayer2D(cPlayer2D);
				eventcontroller->spawnEnemies(skeleton);
				i++;
			}
			else
				break;
		}
	}
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
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_monster.ogg"), 10, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Fire.ogg"), 11, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\hitSound.ogg"), 12, true);

	cSoundController->LoadSound(FileSystem::getPath("Sounds\\enemyMelee.ogg"), 13, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\skeletonAttack.ogg"), 14, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\bobRangeAttack.ogg"), 15, true);

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
	cSoundController->SetMasterVolume(0.03f);



	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{
	cPlayer2D->Update(dElapsedTime);

	if (CInventoryManager::GetInstance()->Check("Sword"))
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->Update(dElapsedTime);
	}

	cMap2D->Update(dElapsedTime);

	//vec2Destination = cPlayer2D->vec2Index;
	//(enemy, player)
	//CAN HEAR SOUND IF ENEMY IS CLOSE
	static double scaleTimer = 0;
	scaleTimer += dElapsedTime;
	if (scaleTimer >= 200)
	{
		hpMulti++;
		scaleTimer = 0;
	}
	vector<CEnemy2D*> enemies = eventcontroller->enemyVector;
	if (cMap2D->GetCurrentLevel() == 1)
	{
		for (int i = 0; i < eventcontroller->enemyVector.size(); i++)
		{
			if (dynamic_cast<Bob*>(eventcontroller->enemyVector[i]) && eventcontroller->enemyVector[i]->bIsActive)
			{
				break;
			}
			else
			{
				for (CEnemy2D* enemy : enemies)
				{
					enemy->bIsActive = false;
				}
				eventcontroller->enemyVector.clear();
				Bob* bob = new Bob(cPlayer2D->vec2Index, hpMulti);
				bob->SetShader("Shader2D_Colour");

				if (bob->Init())
				{
					bob->SetPlayer2D(cPlayer2D);
					eventcontroller->spawnEnemies(bob);
				}
			}
		}
	}
	float distance = cPhysics2D.CalculateDistance(cPlayer2D->vec2Index, enemies[0]->getVec2Index());
	for (CEnemy2D* enemy : enemies)
	{
		if (cPhysics2D.CalculateDistance(cPlayer2D->vec2Index, enemy->getVec2Index()) < distance)
			distance = cPhysics2D.CalculateDistance(cPlayer2D->vec2Index, enemy->getVec2Index());
	}
	cSoundController->Update(dElapsedTime, distance); // update with shortest enemy distance in the frame

	float trackingPosX = cPlayer2D->vec2Index.x + (cPlayer2D->vec2NumMicroSteps.x / CSettings::GetInstance()->NUM_STEPS_PER_TILE_XAXIS);
	float trackingPosY = cPlayer2D->vec2Index.y + (cPlayer2D->vec2NumMicroSteps.y / CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS);
	camera->Update(dElapsedTime, glm::vec2(trackingPosX, trackingPosY));

	cGUI_Scene2D->Update(dElapsedTime);

	

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

	//cout << "CURRENT LEVEL IS " << cMap2D->GetCurrentLevel() << endl;
	eventcontroller->update(dElapsedTime);

	octopusCount = 0;
	chickenCount = 0;
	spiderCount = 0;
	skeletonCount = 0;
	for (int i = 0; i < eventcontroller->enemyVector.size(); i++)
	{
		if (dynamic_cast<Octopus*>(eventcontroller->enemyVector[i]) && eventcontroller->enemyVector[i]->bIsActive)
			octopusCount++;
		if (dynamic_cast<Chicken*>(eventcontroller->enemyVector[i]) && eventcontroller->enemyVector[i]->bIsActive)
			chickenCount++;
		if (dynamic_cast<Spider*>(eventcontroller->enemyVector[i]) && eventcontroller->enemyVector[i]->bIsActive)
			spiderCount++;
		if (dynamic_cast<Skeleton*>(eventcontroller->enemyVector[i]) && eventcontroller->enemyVector[i]->bIsActive)
			skeletonCount++;
	}
	static double octopusSpawnTime = 0;
	octopusSpawnTime += dElapsedTime;
	if (octopusSpawnTime >= 60 && cMap2D->GetCurrentLevel() == 0)
	{
		if (availableRandomSpawn() && octopusCount < 2)
		{
			Octopus* octo = new Octopus(glm::vec2(randX, randY), hpMulti);
			octo->SetShader("Shader2D_Colour");
			{
				if (octo->Init())
				{
					octo->SetPlayer2D(cPlayer2D);
					eventcontroller->spawnEnemies(octo);
					octopusCount++;
				}
			}
		}
		octopusSpawnTime = 0;
	}
	static double chickenSpawnTime = 0;
	chickenSpawnTime += dElapsedTime;
	if (chickenSpawnTime >= 30 && cMap2D->GetCurrentLevel() == 0)
	{
		if (availableRandomSpawn() && chickenCount < 10)
		{
			Chicken* chicken = new Chicken(glm::vec2(randX, randY));
			chicken->SetShader("Shader2D_Colour");
			if (chicken->Init())
			{
				chicken->SetPlayer2D(cPlayer2D);
				eventcontroller->spawnEnemies(chicken);
				chickenCount++;
			}
		}
		chickenSpawnTime = 0;
	}
	static double spiderSpawnTime = 0;
	spiderSpawnTime += dElapsedTime;
	if (spiderSpawnTime >= 90 && cMap2D->GetCurrentLevel() == 0)
	{
		if (availableRandomSpawn() && spiderCount < 4)
		{
			Spider* spider = new Spider(glm::vec2(randX, randY), hpMulti);
			spider->SetShader("Shader2D_Colour");

			if (spider->Init())
			{
				spider->SetPlayer2D(cPlayer2D);
				eventcontroller->spawnEnemies(spider);
				spiderCount++;
			}
			spiderSpawnTime = 0;
		}
	}
	static double skeletonSpawnTime = 0;
	skeletonSpawnTime += dElapsedTime;
	if (skeletonSpawnTime >= 10 && cMap2D->GetCurrentLevel() == 0)
	{
		if (availableRandomSpawn() && skeletonCount < 2)
		{
			Skeleton* skeleton = new Skeleton(glm::vec2(randX, randY), hpMulti);
			skeleton->SetShader("Shader2D_Colour");

			if (skeleton->Init())
			{
				skeleton->SetPlayer2D(cPlayer2D);
				eventcontroller->spawnEnemies(skeleton);
				skeletonCount++;
			}
			skeletonSpawnTime = 0;
		}
	}
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
	glClearColor(50/256.f, 50/256.f, 50/256.f, 1.0f);
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

	if (CInventoryManager::GetInstance()->Check("Sword") && cPlayer2D->getAttacking())
	{
		CSword2D* sword = dynamic_cast<CSword2D*>(CInventoryManager::GetInstance()->GetItem("Sword"));
		sword->PreRender();
		sword->Render();
		sword->PostRender();
	}

	cPlayer2D->PreRender();
	cPlayer2D->Render();
	cPlayer2D->PostRender();

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

bool CScene2D::availableRandomSpawn()
{
	randX = rand() % 100;
	randY = rand() % 100;
	if (cMap2D->GetMapInfo(randX, randY, true, 0) < 100) // to not random spawn into collidable walls
		return true;
	else
		availableRandomSpawn();

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


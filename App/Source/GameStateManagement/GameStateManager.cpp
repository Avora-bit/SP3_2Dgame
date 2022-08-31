#include "GameStateManager.h"

#include <iostream>

using namespace std;

/**
 @brief Constructor
 */
CGameStateManager::CGameStateManager(void)
	: activeGameState(nullptr)
	, nextGameState(nullptr)
	, prevGameState(nullptr)
	, pauseGameState(nullptr)
	, craftingGameState(nullptr)
	, inventoryGameState(nullptr)
{

	
	cPlayer2D = CPlayer2D::GetInstance();
}

/**
 @brief Destructor
 */
CGameStateManager::~CGameStateManager(void)
{
}

/**
 @brief Destroy this class instance
 */
void CGameStateManager::Destroy(void)
{
	// Set the handlers to CGameState to nullptr
	activeGameState = nullptr;
	nextGameState = nullptr;
	prevGameState = nullptr;
	pauseGameState = nullptr;
	craftingGameState = nullptr;
	inventoryGameState = nullptr;

	// Delete all scenes stored and empty the entire map
	std::map<std::string, CGameStateBase*>::iterator it, end;
	end = GameStateMap.end();
	for (it = GameStateMap.begin(); it != end; ++it)
	{
		delete it->second;
	}
	// Clear the map
	GameStateMap.clear();
}

/**
 @brief Update this class instance
 */

//TRY HERE
bool CGameStateManager::Update(const double dElapsedTime)
{
	// Check for change of scene

	//if (craftingGameState != nullptr)
	//{
	//	for (int i = 0; i < 9; i++)
	//	{
	//		if (cCraftingState->returnbutnumval(i) != 0)
	//		{
	//			cout << "INDEX " << i << " is " << craftingGameState->returnbutnumval(i) << endl;
	//			//cout << "CURRENT GAME STATE " << craftingGameState->returnbutnumval << endl;
	//		}
	//	}
	//}

	if (nextGameState != activeGameState)
	{
		if (activeGameState)
		{

			activeGameState->Destroy();
		}
		
		// Set the previous CGameState
		prevGameState = activeGameState;
		// Set the new active CGameState
		activeGameState = nextGameState;
		// Init the new active CGameState
		activeGameState->Init();
	}

	// Update the active CGameState
	if (activeGameState)
	{
		//cout << "activeGameState->Update" << endl;
		if (activeGameState->Update(dElapsedTime) == false)
			return false;
		else
		{
			if (pauseGameState)
			{
				//cout << "pauseGameState->Update" << endl;
				pauseGameState->Update(dElapsedTime);
			}
			if (craftingGameState)
			{
				//cout << "pauseGameState->Update" << endl;
				craftingGameState->Update(dElapsedTime);
			}
			if (inventoryGameState)
			{
				//cout << "pauseGameState->Update" << endl;
				inventoryGameState->Update(dElapsedTime);
			}
		}
	}


	return true;
}

/**
 @brief Render this class instance
 */
void CGameStateManager::Render(void)
{
	if (activeGameState)
		activeGameState->Render();
	if (pauseGameState)
		pauseGameState->Render();
	if (craftingGameState)
		craftingGameState->Render();
	if (inventoryGameState)
		inventoryGameState->Render();
}

/**
 @brief Add a new CGameState to this
 @param _name A const std::string& variable which is the name of the CGameState 
 @param _scene A CGameState* variable which is the CGameState
 */
bool CGameStateManager::AddGameState(const std::string& _name, CGameStateBase* _scene)
{
	// Check if this _name already exists in the map...
	if (CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::AddGameState - Duplicate scene name provided" << endl;
		return false;
	}

	// Check if the _scene is valid
	if (_scene == nullptr)
	{
		cout << "CGameStateManager::AddGameState - Invalid scene pointer" << endl;
		return false;
	}

	// If nothing is wrong, then add the scene to our map
	GameStateMap[_name] = _scene;

	return true;
}

/**
 @brief Remove a CGameState from this
 @param _name A const std::string& variable which is the name of the CGameState
 */
bool CGameStateManager::RemoveGameState(const std::string& _name)
{
	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::RemoveGameState - scene name does not exists" << endl;
		return false;
	}

	// Find the CGameState*
	CGameStateBase* target = GameStateMap[_name];
	if (target == activeGameState || target == nextGameState)
	{
		cout << "Unable to remove active/next scene" << endl;
		return false;
	}

	// Delete and remove from our map
	delete target;
	GameStateMap.erase(_name);

	return true;
}

/**
 @brief Set a CGameState to be active
 @param _name A const std::string& variable which is the name of the CGameState
 */
bool CGameStateManager::SetActiveGameState(const std::string& _name)
{
	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::SetActiveGameState - scene name does not exists" << endl;
		return false;
	}

	// Scene exist, set the next scene pointer to that scene
	nextGameState = GameStateMap[_name];

	return true;
}

bool CGameStateManager::CheckGameStateExist(const std::string& _name)
{
	return GameStateMap.count(_name) != 0;
}

bool CGameStateManager::SetPauseGameState(const std::string& _name)
{
	// Toggle to nullptr if pauseGameState already is in use
	if (pauseGameState != nullptr)
	{
		pauseGameState = nullptr;
		return true;
	}

	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::SetPauseGameState - scene name does not exists" << endl;
		return false;
	}

	// Scene exist, set the next scene pointer to that scene
	pauseGameState = GameStateMap[_name];
	// Init the new pause CGameState
	pauseGameState->Init();

	return true;
}

bool CGameStateManager::SetCraftingGameState(const std::string& _name)
{
	// Toggle to nullptr if crafting state already is in use
	if (craftingGameState != nullptr)
	{
		for (int i = 0; i < 9; i++)
		{
			if (craftingGameState->returnbutnumval(i) != 0)
			{
				for (int n = 0; n < 9; n++)
				{
					if (cPlayer2D->getitemval(n) == 0
						|| 
						(cPlayer2D->getitemval(n) == craftingGameState->returnbutnumval(i)
							&& cPlayer2D->getitem(n).getquantity() < 5 
							&& craftingGameState->returnbutnumval(i) != 50
							&& craftingGameState->returnbutnumval(i) != 102)
						)
					{
						cPlayer2D->setitem(n, craftingGameState->returnbutnumval(i));
						cPlayer2D->setitemquantity(n, craftingGameState->return_butnumQuantity(i) + cPlayer2D->getitem(n).getquantity());
						craftingGameState->setquantity(i, 0);
						break;
					}
				}
			}
		}


		craftingGameState = nullptr;
		return true;
	}

	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::SetCraftingGameState - scene name does not exists" << endl;
		return false;
	}

	// Scene exist, set the next scene pointer to that scene
	craftingGameState = GameStateMap[_name];
	// Init the new pause CGameState
	craftingGameState->Init();

	return true; 
}

bool CGameStateManager::SetInventoryGameState(const std::string& _name)
{
	// Toggle to nullptr if pauseGameState already is in use
	if (inventoryGameState != nullptr)
	{
		inventoryGameState = nullptr;
		return true;
	}

	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::SetInventoryState - scene name does not exists" << endl;
		return false;
	}

	// Scene exist, set the next scene pointer to that scene
	inventoryGameState = GameStateMap[_name];
	// Init the new pause CGameState
	inventoryGameState->Init();

	return true;
}

void CGameStateManager::OffPauseGameState(void)
{
	pauseGameState = nullptr;
}

void CGameStateManager::OffCraftingGameState(void)
{
	
	

	craftingGameState = nullptr;
}

void CGameStateManager::OffInventoryGameState(void)
{
	inventoryGameState = nullptr;
}

CGameStateBase* CGameStateManager::returnCraftingState()
{
	return craftingGameState;
}

CGameStateBase* CGameStateManager::returnInventoryState()
{
	return inventoryGameState;
}


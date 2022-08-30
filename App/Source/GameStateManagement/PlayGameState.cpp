// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "PlayGameState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"


#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CPlayGameState::CPlayGameState(void)
	: cScene2D(NULL)
{

}

/**
 @brief Destructor
 */
CPlayGameState::~CPlayGameState(void)
{

}

/**
 @brief Init this class instance
 */
bool CPlayGameState::Init(void)
{
	cout << "CPlayGameState::Init()\n" << endl;

	cGameManager = CGameManager::GetInstance();
	cGameManager->Init();

	//cInventoryState = CInventoryState::GetInstance();
	//cInventoryState->Init();

	


	// Initialise the cScene2D instance
	cScene2D = CScene2D::GetInstance();
	if (cScene2D->Init() == false)
	{
		cout << "Failed to load Scene2D" << endl;
		return false;
	}

	return true;
}

/**
 @brief Update this class instance
 */
bool CPlayGameState::Update(const double dElapsedTime)
{
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		CGameStateManager::GetInstance()->OffCraftingGameState();
		CGameStateManager::GetInstance()->OffInventoryGameState();

		return true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_F10))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading PauseState" << endl;
		CGameStateManager::GetInstance()->SetPauseGameState("PauseState");
	}
	//set crafting screen
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_Q))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		CGameStateManager::GetInstance()->SetCraftingGameState("CraftingState");
		//CGameStateManager::GetInstance()->OffInventoryGameState();

		if (CGameStateManager::GetInstance()->returnInventoryState() != nullptr)
		{

			CGameStateManager::GetInstance()->SetInventoryGameState("InventoryState");
		}

	}
	//set inventory screen
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_I))
	{
		CKeyboardController::GetInstance()->Reset();


		CGameStateManager::GetInstance()->SetInventoryGameState("InventoryState");
		//CGameStateManager::GetInstance()->OffCraftingGameState();

		if (CGameStateManager::GetInstance()->returnCraftingState() != nullptr)
		{

			CKeyboardController::GetInstance()->Reset();
			CGameStateManager::GetInstance()->SetCraftingGameState("CraftingState");
		}


	}
	else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_A)
		|| CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S)
		|| CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_D)
		|| CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W))
		{
			if (CGameStateManager::GetInstance()->returnCraftingState() != nullptr)
			{

				CKeyboardController::GetInstance()->Reset();
				CGameStateManager::GetInstance()->SetCraftingGameState("CraftingState");
			}


			if (CGameStateManager::GetInstance()->returnInventoryState() != nullptr)
			{

				CKeyboardController::GetInstance()->Reset();
				CGameStateManager::GetInstance()->SetInventoryGameState("InventoryState");
			}
		}

	if (cGameManager->bPlayerLost)
	{
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading GameOverState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("GameOverState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		CGameStateManager::GetInstance()->OffCraftingGameState();
		CGameStateManager::GetInstance()->OffInventoryGameState();


		return true;
	}
	if (cGameManager->bPlayerWon)
	{
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading VictoryState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("VictoryState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		CGameStateManager::GetInstance()->OffCraftingGameState();
		CGameStateManager::GetInstance()->OffInventoryGameState();


		return true;
	}
	// Call the cScene2D's Update method
	cScene2D->Update(dElapsedTime);

	return true;
}

/**
 @brief Render this class instance
 */
void CPlayGameState::Render(void)
{
	//cout << "CPlayGameState::Render()\n" << endl;

	// Call the cScene2D's Pre-Render method
	cScene2D->PreRender();

	// Call the cScene2D's Render method
	cScene2D->Render();

	// Call the cScene2D's PostRender method
	cScene2D->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CPlayGameState::Destroy(void)
{
	cout << "CPlayGameState::Destroy()\n" << endl;

	// Destroy the cScene2D instance
	if (cScene2D)
	{
		cScene2D->Destroy();
		cScene2D = NULL;
	}
}



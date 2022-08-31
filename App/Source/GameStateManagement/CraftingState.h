#pragma once

/**
 CCraftingState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

 // Include ImageLoader
#include "System\ImageLoader.h"

#include "GameStateBase.h"

#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"

#include "Inputs/MouseController.h"

#include <fstream>
#include <vector>
#include <string>
#include "../Scene2D/RecipeBook.h"
#include "../Scene2D/Recipe.h"


#include"../Scene2D/GUI_Scene2D.h"

#include "../Scene2D/slot.h"
#include "../Scene2D/Map2D.h"


#include "../Scene2D/Player2D.h"
#include "GameControl/Settings.h"

#include "../Scene2D/InventoryManager.h"

 // Include IMGUI
 // Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

#include "DesignPatterns\SingletonTemplate.h"

#include "../Scene2D/Sword2D.h"


#include "../Scene2D/Hilt2D.h"
#include "../Scene2D/PlatinumHilt2D.h"
#include "../Scene2D/WoodenHilt2D.h"
#include "../Scene2D/IronHilt2D.h"

#include "../Scene2D/Blade2D.h"
#include "../Scene2D/KatanaBlade2D.h"
#include "../Scene2D/DaggerBlade2D.h"
#include "../Scene2D/CleaverBlade2D.h"
#include "../Scene2D/RustyBlade2D.h"

class CCraftingState : public CGameStateBase, public CSingletonTemplate<CCraftingState>
{
	friend CSingletonTemplate<CCraftingState>;

public:
	// Constructor
	CCraftingState(void);
	// Destructor
	~CCraftingState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);


	int returnbutnumval(int arr);

	int return_butnumQuantity(int arr);

	void setbutnumvalto(int arr, int val);


	void setquantity(int arr, int quantity);
	//butnum[i].setitemID(0);

	CSword2D* getsword();

protected:
	ImGuiPayload ig;

	CMouseController* cMouseController;


	CInventoryManager* cInventoryManager;
	CPlayer2D* cPlayer2D;
	
	CMap2D* cMap2D;

	CHilt2D* hilt;
	CBlade2D* blade;
	CSword2D* sword;

	CSettings* cSettings;

	CGUI_Scene2D* hotbar;

	CImageLoader* il;

	RecipeBook* recipebook;

	Recipe gridrecipe;

	slot butnum[18];

	slot output;
};

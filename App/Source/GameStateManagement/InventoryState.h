#pragma once

/**
 CInventoryState
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

#include"../Scene2D/GUI_Scene2D.h"
#include "../Scene2D/slot.h"
#include "../Scene2D/Map2D.h"


#include "GameControl/Settings.h"


#include "../Scene2D/Player2D.h"

 // Include IMGUI
 // Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

class CInventoryState : public CGameStateBase, public CSingletonTemplate<CInventoryState>
{
public:
	// Constructor
	CInventoryState(void);
	// Destructor
	~CInventoryState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);


	

protected:
	ImGuiPayload ig;

	CMouseController* cMouseController;

	CPlayer2D* cPlayer2D;
	
	CGUI_Scene2D* hotbar;

	CImageLoader* il;

	CMap2D* cMap2D;

	CSettings* cSettings;

	int itemcounter;

	slot butnum[9];

};

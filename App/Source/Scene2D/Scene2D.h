/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#include "DesignPatterns\SingletonTemplate.h"
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include vector
#include <vector>

// Include Keyboard controller
#include "Inputs\KeyboardController.h"
#include "Inputs\MouseController.h"

#include "..\SoundController\SoundController.h"

// Map2D to check player's movements and actions
#include "Map2D.h"
#include "Player2D.h"
#include "Shivs2D.h"
#include "Enemy2D.h"
#include "Octopus.h"
#include "GUI_Scene2D.h"
#include "Camera.h"

#include "GameManager.h"

#include "../GameStateManagement/InventoryState.h"

// Add your include files here

class CScene2D : public CSingletonTemplate<CScene2D>
{
	friend CSingletonTemplate<CScene2D>;
public:
	// Init
	bool Init( const unsigned int uiNumLevels = 1,
			   const unsigned int uiNumRows = 24,
			   const unsigned int uiNumCols = 32);

	// Update
	bool Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	float returnmusicvol();
	float returnsoundvol();


	void setsoundvol(float vol);
	void setmusicvol(float vol);

protected:
	// The handler containing the instance of the 2D map
	CMap2D* cMap2D;

	CPlayer2D* cPlayer2D;

	CShivs2D* CShivs2D;

	vector<CEntity2D*> enemyVector;

	Camera* camera;

	CGUI_Scene2D* cGUI_Scene2D;

	CGameManager* cGameManager;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// Mouse Controller singleton instance
	CMouseController* cMouseController;

	CSoundController* cSoundController;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// Add your variables and methods here.

	float m_fProgressBar;
	ISound* musicsfx;

	float soundVol;
	float musicVol;




	// Constructor
	CScene2D(void);
	// Destructor
	virtual ~CScene2D(void);
};


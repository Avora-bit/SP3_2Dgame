#pragma once

/**
 CCraftingState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"

#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"

#include "Inputs/MouseController.h"

#include <fstream>
#include <vector>
#include "../Scene2D/RecipeBook.h"
#include "../Scene2D/Recipe.h"



#include <string>


 // Include IMGUI
 // Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

class CCraftingState : public CGameStateBase
{
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

protected:
	ImGuiPayload ig;

	CMouseController* cMouseController;

	struct ButtonData
	{
		std::string fileName;
		unsigned textureID;
		unsigned int itemID;


		void loadimagebasedID(int itemid)
		{
			switch (itemid)
			{

			case 1:
				fileName = "Image\\Sp3Images\\Base\\stick.png";
				break;
			case 2:
				fileName = "Image\\Sp3Images\\Base\\wood.png";
				break;

			case 0:
				fileName = "Image\\Sp3Images\\blank_output.png";
				break;

			case 6:
				fileName = "Image\\Sp3Images\\Weapons\\sword.png";
				break;

			default:
				break;
			}



				
		}


		int getitemID()
		{
			return itemID;
		}

		
	};

	//int butnum[];



	RecipeBook* recipebook;

	Recipe gridrecipe;

	ButtonData butnum[9];

	ButtonData output;
};

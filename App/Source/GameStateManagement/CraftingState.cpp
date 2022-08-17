// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "CraftingState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include "../SoundController/SoundController.h"

#include <iostream>
#include <string>

using namespace std;

/**
 @brief Constructor
 */
CCraftingState::CCraftingState(void)
	//: background(NULL)
{

}

//text file reader for minecraft like recipes

/*RecipeBook* recipebook = new RecipeBook("Recipes.txt");
recipebook->CreateRecipe();

recipebook->PrintBook();
Recipe gridrecipe;

//column zero is always 0
gridrecipe.SetRecipeIndex(0, 0);

//set the ingredients required
gridrecipe.SetRecipeIndex(1, 2);
gridrecipe.SetRecipeIndex(2, 2);
gridrecipe.SetRecipeIndex(3, 2);
gridrecipe.SetRecipeIndex(4, 2);
gridrecipe.SetRecipeIndex(5, 2);
gridrecipe.SetRecipeIndex(6, 2);
gridrecipe.SetRecipeIndex(7, 2);
gridrecipe.SetRecipeIndex(8, 2);
gridrecipe.SetRecipeIndex(9, 4);
//hardcode value for recipe
std::cout << recipebook->CheckRecipe(gridrecipe) << std::endl;

//create large vector for all recipes

//create temporary recipe class
//fill recipe with recipe from crafting grid

//compare temporary recipe with all recipes
//get result of the comparison and check if its true


//delete all new arrays
delete recipebook;*/

/**
 @brief Destructor
 */
CCraftingState::~CCraftingState(void)
{

}

/**
 @brief Init this class instance
 */
bool CCraftingState::Init(void)
{
	cout << "CCraftingState::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);


	cMouseController = CMouseController::GetInstance();
	/*VolumeIncreaseButtonData.fileName = "Image\\GUI\\VolumeIncreaseButton2.png";
	VolumeIncreaseButtonData.textureID = il->LoadTextureGetID(VolumeIncreaseButtonData.fileName.c_str(), false);
	VolumeDecreaseButtonData.fileName = "Image\\GUI\\VolumeDecreaseButton2.png";
	VolumeDecreaseButtonData.textureID = il->LoadTextureGetID(VolumeDecreaseButtonData.fileName.c_str(), false);*/


	guiscene2d = CGUI_Scene2D::GetInstance();

	il = CImageLoader::GetInstance();
	
	recipebook = new RecipeBook("Recipes.txt");
	recipebook->CreateRecipe();
	recipebook->PrintBook();

	gridrecipe.SetRecipeIndex(0, 0);

	for (int i = 0; i < 12; i++)
	{
		if (i < 9)
		{
			if (i % 2 == 0)
			{
				butnum[i].itemID = 1;
			}
			else
			{
				//butnum[i].itemID = 2;
				butnum[i].itemID = 0;


			}
		}
		else
		{
			butnum[i].itemID = guiscene2d->return_hbcellid(i - 9);

		}

		gridrecipe.SetRecipeIndex(i + 1, butnum[i].itemID);
		butnum[i].loadimagebasedID(butnum[i].itemID);
		butnum[i].textureID = il->LoadTextureGetID(butnum[i].fileName.c_str(), false);
	}

	output.itemID = 0;
	output.loadimagebasedID(output.itemID);
	output.textureID = il->LoadTextureGetID(output.fileName.c_str(), false);

	return true;
}

/**
 @brief Update this class instance
 */
bool CCraftingState::Update(const double dElapsedTime)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;

	float buttonWidth = 256;
	float buttonHeight = 128;

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Crafting", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth/2.0 - buttonWidth/2.0, 
			CSettings::GetInstance()->iWindowHeight/3.0));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;

		// Display the FPS
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Crafting Menu");
		
		// by tohdj
		char y[9];
		for (int n = 0; n < 12; n++)
		{
			ImGui::PushID(n);

			//don't break line if doesn't reach 3 cells
			if ((n % 3) != 0)
				ImGui::SameLine();


			string x = to_string(n);
			//string x = to_string(butnum[n].itemID);

			strcpy(y, x.c_str());

			//ImGui::Button(y, ImVec2(50, 50));
			//construct 9 buttons

			/*ImageButton(ImTextureID user_texture_id, const ImVec2 & size, const ImVec2 & uv0 = ImVec2(0, 0), const ImVec2 & uv1 = ImVec2(1, 1),
				int frame_padding = -1, const ImVec4 & bg_col = ImVec4(0, 0, 0, 0), const ImVec4 & tint_col = ImVec4(1, 1, 1, 1)); */

			if (n >= 9)
			{
				ImGui::ImageButton((ImTextureID)butnum[n].textureID, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1),
					-1, ImVec4(1, 1, 0, 1) );

				/*ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0 - buttonWidth / 2.0,
					CSettings::GetInstance()->iWindowHeight / 2.0));*/

			}
			else
			{
				ImGui::ImageButton((ImTextureID)butnum[n].textureID, ImVec2(50, 50));

			}

			
			if (butnum[n].itemID != 0)
			{
				// Our buttons are both drag sources and drag targets here!
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					// Set payload to carry the index of our item (could be anything)
					//&n is to get the data directly from IMGUI
					ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));
					ImGui::Text("Check %s", y);
					ImGui::EndDragDropSource();
				}
			}
			if (ImGui::BeginDragDropTarget())
			{
				//get with the id
				//when mouse is released
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;

					//swap images and itemId inside
					ButtonData tmp = butnum[n];
					butnum[n] = butnum[payload_n];
					butnum[payload_n] = tmp;

					//set new values for each index
					for (int i = 1; i < 10; i++)
					{
						gridrecipe.SetRecipeIndex(i, butnum[i - 1].itemID);
						cout << "Number " << i << " is " << gridrecipe.GetRecipeIndex(i) << endl;
					}

					cout << endl;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();

		}
		

		//print out the value of recipebook index 0
		cout << recipebook->CheckRecipe(gridrecipe) << endl;

		//render the output button
		/*ImGui::ImageButton((ImTextureID)output.textureID, ImVec2(50, 50));
		output.itemID = recipebook->CheckRecipe(gridrecipe);
		output.loadimagebasedID(output.itemID);
		output.textureID = il->LoadTextureGetID(output.fileName.c_str(), false);*/
		
		/*float currentvol = 0;
		if (ImGui::SliderFloat("Music", &currentvol, 0, 100))
		{

		}*/

		ImGui::End();
	}

	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "UnLoading CraftState" << endl;
		CGameStateManager::GetInstance()->SetCraftingGameState(nullptr);
		//CGameStateManager::GetInstance()->OffCraftingGameState();

		return true;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CCraftingState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//cout << "CCraftingState::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void CCraftingState::Destroy(void)
{
	// cout << "CCraftingState::Destroy()\n" << endl;
	delete recipebook;


	/*for (int i = 0; i < 12; i++)
	{
		delete butnum[i];
	}*/
	recipebook = nullptr;

	/*delete il;
	il = nullptr;*/
}

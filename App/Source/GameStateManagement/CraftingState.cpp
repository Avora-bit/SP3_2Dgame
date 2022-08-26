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
	: cPlayer2D(NULL)
	, cMouseController(NULL)
	, cSettings(NULL)
	, guiscene2d(NULL)
	, il(NULL)
	, recipebook(NULL)
{

}



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
	cPlayer2D = CPlayer2D::GetInstance();

	cMouseController = CMouseController::GetInstance();
	/*VolumeIncreaseButtonData.fileName = "Image\\GUI\\VolumeIncreaseButton2.png";
	VolumeIncreaseButtonData.textureID = il->LoadTextureGetID(VolumeIncreaseButtonData.fileName.c_str(), false);
	VolumeDecreaseButtonData.fileName = "Image\\GUI\\VolumeDecreaseButton2.png";
	VolumeDecreaseButtonData.textureID = il->LoadTextureGetID(VolumeDecreaseButtonData.fileName.c_str(), false);*/


	cSettings = CSettings::GetInstance();

	guiscene2d = CGUI_Scene2D::GetInstance();

	il = CImageLoader::GetInstance();
	
	recipebook = new RecipeBook("Recipes.txt");
	recipebook->CreateRecipe();
	recipebook->PrintBook();

	gridrecipe.SetRecipeIndex(0, 0);

	for (int i = 0; i < 19; i++)
	{
		if (i < 9)
		{
			//set crafting system to 0
			butnum[i].setitemID(0);
			gridrecipe.SetRecipeIndex(i + 1, butnum[i].getitemID());
		}
		else if (i == 18)
		{
			butnum[i].setitemID(0);

		}
		else if(i != 18 && i >=9)
		{
			//hotbar
			//butnum[i].setitemID(guiscene2d->return_hbcellid(i - 9));
			butnum[i].setitemID(cPlayer2D -> getitemval(i - 9));
			butnum[i].settextureID(butnum[i].getitemID());


			butnum[i].AddQuantity(cPlayer2D->getitem(i - 9).getquantity());

			cout << "ID IS " << butnum[i].gettextureID() << endl;


		}

		butnum[i].settextureID(butnum[i].getitemID());



		//butnum[i].loadimagebasedID(butnum[i].getitemID(), il);
		//butnum[i].Init(il);

		//MapOfTextureIDs.at(butnum[i].getitemID())
	}

	

	return true;
}

/**
 @brief Update this class instance
 */
bool CCraftingState::Update(const double dElapsedTime)
{
	// Calculate the relative scale to our default windows width
	const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	const float relativeScale_y = cSettings->iWindowHeight / 600.0f;

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


		//DISPLAY WORDS
		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("QuantityText", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0 - buttonWidth / 10.0,
			CSettings::GetInstance()->iWindowHeight / 3.0));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style2 = ImGui::GetStyle();
		style2.FrameRounding = 200.0f;

		// by tohdj
		for (int n = 9; n < 18; n++)
		{
			ImGui::PushID(n);

			//don't break line if doesn't reach 3 cells
			if ((n % 3) != 0)
				ImGui::SameLine();

			//string x = to_string(n);
			//strcpy(y, x.c_str());

			//render the bar yellow if it's hotbar

			//ImGui::TextColored(ImVec4(1, 1, 0, 1), "H");
			/*ImGui::CalcTextSize("H", NULL, false, 10);*/
			ImGui::SetWindowFontScale(4.f);
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "% d", butnum[n].getquantity()
			/*cInventoryItem->GetCount(), cInventoryItem->GetMaxCount()*/);

			ImGui::PopID();
		}
		ImGui::End();





		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Crafting", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth/2.0 - buttonWidth/2.0, 
			(CSettings::GetInstance()->iWindowHeight/3.0) - 100));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;

		// Display the FPS
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Crafting Menu");
		
		// by tohdj
		char y[19];
		for (int n = 0; n < 19; n++)
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

			if (n >= 9 && n < 18)
			{
				ImGui::ImageButton((ImTextureID)butnum[n].gettextureID(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1),
					-1, ImVec4(1, 1, 0, 1));
			}
			//IF ITEM IS OUTPUT SLOT
			else if(n == 18)
			{
				ImGui::ImageButton((ImTextureID)butnum[n].gettextureID(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1),
					-1, ImVec4(1, 0, 0, 1));
				//OUTPUT INVENTORY
				butnum[n].setitemID( recipebook->CheckRecipe(gridrecipe));
				butnum[n].settextureID(butnum[n].getitemID());

			

				//BRING ITEM TO INVENTORY	
				if (ImGui::IsItemHovered())
				{
					if (cMouseController->IsButtonDown(0))
					{
						for (int x = 9; x < 18; x++)
						{
							if (butnum[x].getitemID() == 0)
							{
								//set in inventiory slot 
								butnum[x].setitemID(butnum[n].getitemID());
								butnum[x].settextureID(butnum[x].getitemID());

								//butnum[x].loadimagebasedID(butnum[x].getitemID(), il);

								//set the inventory to the item
								cPlayer2D->setitem(x - 9, butnum[x].getitemID());
								//empty everything in the crafting slot
								for (int i = 0; i < 9; i++)
								{
									butnum[i].setitemID(0);
									butnum[i].settextureID(butnum[i].getitemID());
								}
								//empty the output slot
								butnum[n].setitemID(0);
								butnum[n].settextureID(butnum[n].getitemID());

								break;
							}
						}
					}
				}

				//butnum[n].loadimagebasedID(butnum[n].getitemID(), il);
			}
			else
			{
				ImGui::ImageButton((ImTextureID)butnum[n].gettextureID(), ImVec2(50, 50));
				gridrecipe.SetRecipeIndex(n + 1, butnum[n].getitemID());

			}

			if (butnum[n].getitemID() != 0)
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
					/*slot tmp = butnum[n];
					butnum[n] = butnum[payload_n];
					butnum[payload_n] = tmp;*/


					int tmp = butnum[n].gettextureID();
					butnum[payload_n].settextureID(butnum[n].gettextureID());
					//butnum[payload_n] = tmp;


					butnum[n].AddQuantity(1);
					butnum[payload_n].SubtractQuantity(1);

					//payload is what is selected to drag
					// n is what it's being dragged to
				
					//if the slot is in the inventory slot
					if (n >= 9 && n < 18)
					{
						//HOTBAR
						if (n < 12)
						{
							guiscene2d->set_hbcellid(n - 9, butnum[n].getitemID());

						}
						cPlayer2D->setitem(n - 9, butnum[n].getitemID());
						//cPlayer2D->getitem(payload_n).SubtractQuantity(1);


					}
					if (payload_n >= 9 && payload_n < 18)
					{
						//HOTBAR
						if (payload_n < 12)
						{
							guiscene2d->set_hbcellid(payload_n - 9, butnum[payload_n].getitemID());

						}
						cPlayer2D->setitem(payload_n - 9, butnum[payload_n].getitemID());
						//cPlayer2D->getitem(payload_n - 9).SubtractQuantity(1);

					}



					//void CPlayer2D::setitem(int arr, int itemid)
					//{
					//	inventorySlots[arr].setitemID(itemid);
					//	inventorySlots[arr].settextureID(inventorySlots[arr].getitemID());
					//	//inventorySlots[arr].loadimagebasedID(inventorySlots[arr].getitemID(), il);

					//}
				}


				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();

		}

		/*for (int i = 0; i < 19; i++)
		{
			cout << "OUTPUT " << i << " IS " << butnum[i].returnstring() << endl;
		}*/

		//cout << "OUTPUT " << butnum[18].getitemID() << endl;


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
	//for (int i = 0; i < 9; i++)
	//{
	//	if (returnbutnumval(i) != 0)
	//	{
	//		cout << "INDEX " << i << " is " << returnbutnumval(i) << endl;

	//		//for (int n = 0; n < 9; n++)
	//		//{
	//		//	if (cPlayer2D->getitemval(n) == 0)
	//		//	{
	//		//		cPlayer2D->setitem(n, cCraftingState->returnbutnumval(i));
	//		//	}
	//		//}
	//		//for (int x = 9; x < 18; x++)
	//		//{
	//		//	if (cCraftingState->returnbutnumval(x) == 0)
	//		//	{
	//		//		cCraftingState->setbutnumvalto(x, cCraftingState->returnbutnumval(i));
	//		//		//cCraftingState->setbutnumvalto0(i);
	//		//	}
	//		//}
	//	}
	//	else
	//	{
	//		cout << "HAS NOTHING" << endl;
	//	}
	//}
	if (guiscene2d)
	{
		guiscene2d = NULL;
	}

	if (cMouseController)
	{
		// We won't delete this since it was created elsewhere
		cMouseController = NULL;
	}

	if (cSettings)
	{
		// We won't delete this since it was created elsewhere
		cSettings = NULL;
	}

	if (cPlayer2D)
	{
		cPlayer2D = NULL;
	}

	delete recipebook;
	recipebook = nullptr;

	delete il;
	il = nullptr;

	/*for (int i = 0; i < 12; i++)
	{
		delete butnum[i];
	}*/
}

int CCraftingState::returnbutnumval(int arr)
{
	return butnum[arr].getitemID();
}

void CCraftingState::setbutnumvalto(int arr, int val)
{
	butnum[arr].setitemID(val);
}

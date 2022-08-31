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
	, hotbar(NULL)
	, il(NULL)
	, recipebook(NULL)
	, sword(NULL)
	, hilt (NULL)
	, blade(NULL)
	, cInventoryManager(NULL)
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
	//cout << "CCraftingState::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	cPlayer2D = CPlayer2D::GetInstance();

	cMouseController = CMouseController::GetInstance();
	/*VolumeIncreaseButtonData.fileName = "Image\\GUI\\VolumeIncreaseButton2.png";
	VolumeIncreaseButtonData.textureID = il->LoadTextureGetID(VolumeIncreaseButtonData.fileName.c_str(), false);
	VolumeDecreaseButtonData.fileName = "Image\\GUI\\VolumeDecreaseButton2.png";
	VolumeDecreaseButtonData.textureID = il->LoadTextureGetID(VolumeDecreaseButtonData.fileName.c_str(), false);*/
	cInventoryManager  = CInventoryManager::GetInstance();

	cSettings = CSettings::GetInstance();

	hotbar = CGUI_Scene2D::GetInstance();

	cMap2D = CMap2D::GetInstance();

	il = CImageLoader::GetInstance();
	
	recipebook = new RecipeBook("Recipes.txt");
	recipebook->CreateRecipe();
	//recipebook->PrintBook();

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
			butnum[i].settextureID(butnum[i].getitemID());
			butnum[i].setquantity(0);
		}
		//IF THE SLOT IS LINKED TO INVENTORY
		else if(i != 18 && i >=9)
		{
			//hotbar
			//butnum[i].setitemID(hotbar->return_hbcellid(i - 9));
			butnum[i].setitemID(cPlayer2D -> getitemval(i - 9));
			butnum[i].settextureID(butnum[i].getitemID());
			butnum[i].setquantity(cPlayer2D->getitem(i - 9).getquantity());

			//cout << "ID IS " << butnum[i].gettextureID() << endl;
		}

		butnum[i].settextureID(butnum[i].getitemID());
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

	
	//IF ITEM IS PICKED UP WHILE IN THIS STATE, UPDATE THE SLOTS
	/*if (cPlayer2D->AddItem())
	{*/
	//	for (int i = 9; i < 18; i++)
	//	{
	//		butnum[i].setitemID(cPlayer2D->getitemval(i - 9));
	//		butnum[i].setquantity(cPlayer2D->getitem(i - 9).getquantity());
	//		butnum[i].settextureID(butnum[i].getitemID());
	//	}
	////}


	{
		static float f = 0.0f;
		static int counter = 0;


		//DISPLAY WORDS
		ImGui::Begin("QuantityText", NULL, window_flags);
		//ImGui::SetWindowPos(ImVec2((CSettings::GetInstance()->iWindowWidth * 0.68) /*- buttonWidth/100.0*/,
		//	(CSettings::GetInstance()->iWindowHeight / 30.0)));				// Set the top-left of the window at (10,10)
		//ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		ImGui::SetWindowPos(ImVec2((CSettings::GetInstance()->iWindowWidth * 0.60),
			(CSettings::GetInstance()->iWindowHeight / 3.4f)));
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
				ImGui::SameLine(0.f, 70.f);
			if ((n % 3) == 0)
			{
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
			}

			ImGui::SetWindowFontScale(2.f);
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "% d", butnum[n].getquantity()
			/*cInventoryItem->GetCount(), cInventoryItem->GetMaxCount()*/);

			ImGui::PopID();
		}
		ImGui::End();
		




		ImGui::Begin("Crafting", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2((CSettings::GetInstance()->iWindowWidth * 0.54) /*- buttonWidth/100.0*/,
			(CSettings::GetInstance()->iWindowHeight / 10.0)));				// Set the top-left of the window at (10,10)
		//ImGui::SetWindowSize(ImVec2(180.0f * relativeScale_x, 25.0f * relativeScale_y));
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;

		// Display the FPS
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Crafting Menu");
		

		for (int n = 9; n < 18; n++)
		{
			cPlayer2D->setitemquantity(n - 9, butnum[n].getquantity());
			cPlayer2D->setitem(n - 9, butnum[n].getitemID());
		}

		// by tohdj
		for (int n = 0; n < 19; n++)
		{
			ImGui::PushID(n);
			//don't break line if doesn't reach 3 cells
			if ((n % 3) != 0)
				ImGui::SameLine(0.f, 40.f);

			//IF ITEM IS IN INVENTORY SLOT
			if (n >= 9 && n < 18)
			{
				ImGui::ImageButton((ImTextureID)butnum[n].gettextureID(), ImVec2(CSettings::GetInstance()->iWindowWidth * 0.05, CSettings::GetInstance()->iWindowWidth * 0.05), ImVec2(0, 0), ImVec2(1, 1),
					9, ImVec4(1, 1, 0, 1));

				if (butnum[n].getitemID() != 0)
				{
					//Discard items
					if (ImGui::IsItemHovered())
					{
						if (cMouseController->IsButtonDown(1) && butnum[n].getquantity() != 0)
						{
							butnum[n].settextureID(butnum[n].getitemID());
							butnum[n].SubtractQuantity(1);

							if (butnum[n].getquantity() == 0)
							{
								butnum[n].setitemID(0);
								butnum[n].settextureID(0);
							}

							//REDUCE THE QUANTITY IN HOTBAR AS WELL
							if (n <= 2)
							{
								hotbar->set_hbcellid(n - 9, butnum[n].getitemID());
							}
							
						}
					}
				}
			}
			//IF ITEM IS OUTPUT SLOT
			else if(n == 18)
			{
				ImGui::ImageButton((ImTextureID)butnum[n].gettextureID(), ImVec2(CSettings::GetInstance()->iWindowWidth * 0.05, CSettings::GetInstance()->iWindowWidth * 0.05), ImVec2(0, 0), ImVec2(1, 1),
					9, ImVec4(1, 0, 0, 1));

				//OUTPUT INVENTORY
				butnum[n].setitemID( recipebook->CheckRecipe(gridrecipe));
				butnum[n].settextureID(butnum[n].getitemID());


				//BRING OUTPUT ITEM TO INVENTORY	
				if (ImGui::IsItemHovered())
				{
					if (cMouseController->IsButtonDown(0) && butnum[n].getitemID() != 0)
					{
						//ADD SWORD
						if (butnum[n].getitemID() == 50)
						{
							sword = new CSword2D(new CWoodenHilt2D(), new CRustyBlade2D());
							cInventoryManager->Add(sword);
						}
						for (int x = 9; x < 18; x++)
						{
							//IF ITEM IS EMPTY
							if (butnum[x].getitemID() == 0)
							{
								//set in inventory slot 
								butnum[x].setitemID(butnum[n].getitemID());
								butnum[x].settextureID(butnum[x].getitemID());
								butnum[x].AddQuantity(1);

								//set the inventory to the item
								cPlayer2D->setitem(x - 9, butnum[x].getitemID());

								//empty everything in the crafting slot
								for (int i = 0; i < 9; i++)
								{
									butnum[i].setitemID(0);
									butnum[i].settextureID(butnum[i].getitemID());
									butnum[i].setquantity(0);
								}

								//empty the output slot
								butnum[n].setitemID(0);
								butnum[n].settextureID(butnum[n].getitemID());
								break;
							}
						}
					}
				}
			}
			else
			{
				ImGui::ImageButton((ImTextureID)butnum[n].gettextureID(), ImVec2(CSettings::GetInstance()->iWindowWidth * 0.05, CSettings::GetInstance()->iWindowWidth * 0.05), ImVec2(0, 0), ImVec2(1, 1),
					9);
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

					//PAYLOAD_N IS WHAT IS SELECTED TO DRAG
					//N IS WHAT IT'S BEEN SELECTED TO BE DRAGGED TO

					//IF ITEM IS DRAGGED INTO CRAFTING SECTION
					if (n < 9)
					{
						if (butnum[n].getitemID() == 0)
						{
							int tmp = butnum[payload_n].getitemID();
							butnum[n].setitemID(tmp);
							butnum[n].settextureID(butnum[n].getitemID());
							butnum[n].AddQuantity(1);

							butnum[payload_n].SubtractQuantity(1);
						}
					}
					//IF ITEM IS DRAGGED INTO INVENTORY SECTION
					else
					{
						if (butnum[n].getquantity() < 5
							&& butnum[n].getitemID() == butnum[payload_n].getitemID())
						{
							int tmp = butnum[payload_n].getitemID();
							butnum[n].setitemID(tmp);
							butnum[n].AddQuantity(1);
							butnum[payload_n].SubtractQuantity(1);
						}
						//IF IT'S DRAGGED TO THE FIRST 3 INVENTORY SLOTS
						if (n < 12)
						{
							hotbar->set_hbcellid(n - 9, butnum[n].getitemID());
						}
						cPlayer2D->setitem(n - 9, butnum[n].getitemID());
					}

					if (payload_n >= 9 && payload_n < 18)
					{
						//HOTBAR
						if (payload_n < 12)
						{
							hotbar->set_hbcellid(payload_n - 9, butnum[payload_n].getitemID());

						}
						cPlayer2D->setitem(payload_n - 9, butnum[payload_n].getitemID());
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();

			//IF QUANTITY IS 0, SET TEXTUREID TO 0 
			if (butnum[n].getquantity() == 0 && n != 18)
			{
				butnum[n].setitemID(0);
				butnum[n].settextureID(0);

			}


		}
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

	if (hotbar)
	{
		hotbar = NULL;
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
}

int CCraftingState::returnbutnumval(int arr)
{
	return butnum[arr].getitemID();
}

int CCraftingState::return_butnumQuantity(int arr)
{
	return butnum[arr].getquantity();
}

void CCraftingState::setbutnumvalto(int arr, int val)
{
	butnum[arr].setitemID(val);
}

void CCraftingState::setquantity(int arr, int quantity)
{
	butnum[arr].setquantity(quantity);
}

CSword2D* CCraftingState::getsword()
{

	return sword;
}



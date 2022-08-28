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

#include "InventoryState.h"

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
CInventoryState::CInventoryState(void)
	//: background(NULL)
{

}

/**
 @brief Destructor
 */
CInventoryState::~CInventoryState(void)
{

}

/**
 @brief Init this class instance
 */
bool CInventoryState::Init(void)
{
	cout << "CInventoryState::Init()\n" << endl;
	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	cMouseController = CMouseController::GetInstance();
	il = CImageLoader::GetInstance();
	
	hotbar = CGUI_Scene2D::GetInstance();
	cPlayer2D = CPlayer2D::GetInstance();

	for (int i = 0; i < 9; i++)
	{
		/*if (i > 2)
		{*/
			butnum[i].setitemID(cPlayer2D->getitemval(i));
			//cout << cPlayer2D->getitem(i).gettextureID() << endl;
			//butnum[i].settextureID(cPlayer2D->getitem(i).gettextureID());
			//butnum[i].settextureID(cPlayer2D->gettextureid(i));
			butnum[i].settextureID(butnum[i].getitemID());


			butnum[i].setquantity(cPlayer2D->getitem(i).getquantity());


			cout << "INVENTORY STATE IS " << butnum[i].gettextureID() << endl;
		//}
		//else
		//{
		//	//the first three items are from the hotbar
		//	butnum[i].setitemID(hotbar->return_hbcellid(i));
		//}

		//butnum[i].loadimagebasedID(butnum[i].getitemID(), il);
		//butnum[i].Init(il);
	}

	return true;
}

/**
 @brief Update this class instance
 */
bool CInventoryState::Update(const double dElapsedTime)
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

	//DISPLAY WORDS
		// Create a window called "Hello, world!" and append into it.
	ImGui::Begin("QuantityText", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2((CSettings::GetInstance()->iWindowWidth * 0.65) /*- buttonWidth/100.0*/,
		(CSettings::GetInstance()->iWindowHeight / 10.0)));				// Set the top-left of the window at (10,10)
	ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

	//Added rounding for nicer effect
	ImGuiStyle& style2 = ImGui::GetStyle();
	style2.FrameRounding = 200.0f;

	// by tohdj
	for (int n = 0; n < 9; n++)
	{
		ImGui::PushID(n);

		//don't break line if doesn't reach 3 cells
		if ((n % 3) != 0)
			ImGui::SameLine();

		ImGui::SetWindowFontScale(4.f);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "% d", butnum[n].getquantity()
		/*cInventoryItem->GetCount(), cInventoryItem->GetMaxCount()*/);

		ImGui::PopID();
	}
	ImGui::End();


	//DISPLAY INVENTORY PICTURES
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Inventory", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2((CSettings::GetInstance()->iWindowWidth * 0.65) /*- buttonWidth/100.0*/,
			(CSettings::GetInstance()->iWindowHeight / 10.0)));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;

		// Display the FPS
		ImGui::SetWindowFontScale(4.f);
		ImGui::TextColored(ImVec4(0, 0, 0, 1), "Inventory Menu");
		
		// by tohdj
		for (int n = 0; n < 9; n++)
		{
			ImGui::PushID(n);

			//don't break line if doesn't reach 3 cells
			if ((n % 3) != 0)
				ImGui::SameLine();

			//render the bar yellow if it's hotbar
			if (n >= 3)
			{
				/*ImageButton(ImTextureID user_texture_id, const ImVec2 & size, const ImVec2 & uv0 = ImVec2(0, 0),
					const ImVec2 & uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4 & bg_col = ImVec4(0, 0, 0, 0),
					const ImVec4 & tint_col = ImVec4(1, 1, 1, 1));*/
				ImGui::ImageButton((ImTextureID)butnum[n].gettextureID(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1),
					-1, ImVec4(1, 1, 0, 1) );
				
			}
			else
			{
				ImGui::ImageButton((ImTextureID)butnum[n].gettextureID(), ImVec2(50, 50));
				
			}

			if (butnum[n].getitemID() != 0)
			{
				//Discard items
				if (ImGui::IsItemHovered())
				{
					//ImGui::Text("Check %s", butnum[n].getitemID());
					if (cMouseController->IsButtonDown(1))
					{
						butnum[n].setitemID(0);
						//butnum[n].loadimagebasedID(butnum[n].getitemID(), il);
						butnum[n].settextureID(butnum[n].getitemID());

						butnum[n].SubtractQuantity(1);
						//break;

						//set hotbar to 0
						if (n <= 2)
						{
							hotbar->set_hbcellid(n, butnum[n].getitemID());
						}

					}
				}

				// Our buttons are both drag sources and drag targets here!
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					//butnum[n].SubtractQuantity(1);

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

					//swap images and itemId inside
					slot tmp = butnum[n];
					butnum[n] = butnum[payload_n];
					butnum[payload_n] = tmp;

					//cPlayer2D->setitem

					//payload is what is selected to drag
					// n is what it's being dragged to
					cPlayer2D->setitem(n, butnum[n].getitemID());
					cPlayer2D->setitem(payload_n, butnum[payload_n].getitemID());

					//set the hotbar to the item
					if (n < 3)
					{
						hotbar->set_hbcellid(n, butnum[n].getitemID());
					}
					if (payload_n < 3)
					{
						hotbar->set_hbcellid(payload_n, butnum[payload_n].getitemID());
					}
					cout << endl;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}
		ImGui::End();




		
	}


	for (int n = 0; n < 9; n++)
	{
		cPlayer2D->setitemquantity(n, butnum[n].getquantity());
		cPlayer2D->setitem(n, butnum[n].getitemID());

		/*cout << "PLAYER QUANTITY IS " << cPlayer2D->getitem(n).getquantity() << endl;*/

	}
	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "UnLoading InventoryState" << endl;
		CGameStateManager::GetInstance()->SetInventoryGameState(nullptr);

		return true;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CInventoryState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//cout << "CInventoryState::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void CInventoryState::Destroy(void)
{
	
	/*delete cPlayer2D;
	cPlayer2D = nullptr;*/

	delete il;
	il = nullptr;

}





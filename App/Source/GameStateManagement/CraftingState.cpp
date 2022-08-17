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
// Include ImageLoader
#include "System\ImageLoader.h"
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

	CImageLoader* il = CImageLoader::GetInstance();
	
	for (int i = 0; i < 9; i++)
	{

		if (i % 2 == 0)
		{
			butnum[i].fileName = "Image\\Sp3Images\\Base\\stick.png";
		}
		else
		{
			butnum[i].fileName = "Image\\Sp3Images\\Base\\wood.png";

		}
		butnum[i].textureID = il->LoadTextureGetID(butnum[i].fileName.c_str(), false);

		cout << "Index " << i << "TexttureID: " << butnum[i].textureID << endl;
	}

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
		for (int n = 0; n < 9; n++)
		{
			ImGui::PushID(n);

			//don't break line if doesn't reach 3 cells
			if ((n % 3) != 0)
				ImGui::SameLine();


			//string x = to_string(butnum[n]);
			string x = to_string(n);

			strcpy(y, x.c_str());

			//ImGui::Button(y, ImVec2(50, 50));
			ImGui::ImageButton((ImTextureID)butnum[n].textureID, ImVec2(50, 50));


			if (cMouseController->IsButtonDown(1))
			{

			}

			// Our buttons are both drag sources and drag targets here!
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Set payload to carry the index of our item (could be anything)
				//&n is to get the data directly from IMGUI
				ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));
				ImGui::Text("Check %s", y);
				ImGui::EndDragDropSource();
			}
			//when mouse is released
			if (ImGui::BeginDragDropTarget())
			{
				//get with the id
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
				{
					//IM_ASSERT(payload->DataSize == sizeof(int));
					IM_ASSERT(payload->DataSize == sizeof(int));

					int payload_n = *(const int*)payload->Data;

					//swap values inside
					unsigned tmp = butnum[n].textureID;
					butnum[n].textureID = butnum[payload_n].textureID;
					butnum[payload_n].textureID = tmp;
				}
				ImGui::EndDragDropTarget();
			}


			ImGui::PopID();


			cout << butnum[n].fileName << endl;
		}






		// By Reagan (wrong)
		/*char y[9];
		for (int i = 0; i < 9; i++)
		{
			string x = to_string(butnum[i]);
			strcpy(y, x.c_str());
			//cout << y << endl;

			//construct 9 buttons
			ImGui::Button(y, ImVec2(50, 50));

			//when hold click, delete the button from imgui
			//put pointer into mouse
			//when hover over item, push item backwards
			//append value of mouse into array
				
			//when mouse is hold
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload(y, &y, (butnum + 1) - butnum);

				//button is dragged along with mouse, but it's stil at it's initial position
				ImGui::Button(y, ImVec2(50, 50));
				ImGui::SetWindowPos(ImVec2(cMouseController->GetMousePositionX() * 0.03,
				cMouseController->GetMousePositionY() * 0.6));
				
				ImGui::EndDragDropSource();
			}

			//if key is released
			if (ImGui::BeginDragDropTarget())
			{
				cout << butnum[i] << endl;
				//ImGui::Get

				///*int payload = ImGui::AcceptDragDropPayload(y);
				if (ImGui::AcceptDragDropPayload(y) != NULL)
				{
					ssert(payload.DataSize == ffi.sizeof"int");
					local numptr = ffi.cast("int*", payload.Data);
					//swap numbers
					//butnum[numptr[0]], butnum[i] = butnum[i], butnum[numptr[0]];
					//butnum[i] = butnum[0];
				}
				ImGui::EndDragDropTarget();
			}
		}*/
		
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
}

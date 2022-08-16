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

	
	for (int i = 0; i < 9; i++)
	{
		butnum[i] = i + 1;
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
		

		// Drag and Drop
	// - If you stop calling BeginDragDropSource() the payload is preserved however it won't have a preview tooltip 
	// (we currently display a fallback "..." tooltip as replacement)
	
		//IMGUI_API bool          BeginDragDropSource(ImGuiDragDropFlags flags = 0);                                      
		// // call when the current item is active. If this return true, you can call SetDragDropPayload() + EndDragDropSource()
		
		//IMGUI_API bool          SetDragDropPayload(const char* type, const void* data, size_t sz, ImGuiCond cond = 0);  
		// // type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear imgui internal types. Data is copied and held by imgui.
		
		//IMGUI_API void          EndDragDropSource();                                                                    
		// // only call EndDragDropSource() if BeginDragDropSource() returns true!
		
		//IMGUI_API bool                  BeginDragDropTarget();                                                          
		// // call after submitting an item that may receive a payload. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget()
		
		//IMGUI_API const ImGuiPayload* AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0);          
		// // accept contents of a given type. If ImGuiDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.

		//IMGUI_API void                  EndDragDropTarget();                                                            
		// // only call EndDragDropTarget() if BeginDragDropTarget() returns true!
		
		//IMGUI_API const ImGuiPayload* GetDragDropPayload();                                                           
		// peek directly into the current payload from anywhere. may return NULL. use ImGuiPayload::IsDataType() to test for the payload type.


		char y[9];
		for (int i = 0; i < 9; i++)
		{
			/*const */string x = to_string(butnum[i]);
			//y[i] = x;

			strcpy(y, x.c_str());
			//cout << y << endl;

			ImGui::Button(y, ImVec2(50, 50));
			if (ImGui::BeginDragDropSource())
			{
				//SetDragDropPayload(const char* type, const void* data, size_t sz, ImGuiCond cond = 0);
				ImGui::SetDragDropPayload(y, y, (butnum + 1) - butnum);


				//when hold click, delete the button from imgui
				//put pointer into mouse
				//when hover over item, push item backwards
				//append value of mouse into array

				ImGui::Button(y, ImVec2(50, 50));
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{

				cout << butnum[i] << endl;

				///*int payload = */ImGui::AcceptDragDropPayload(y);
				if (ImGui::AcceptDragDropPayload(y) != NULL)
				{
					/*ssert(payload.DataSize == ffi.sizeof"int");
					local numptr = ffi.cast("int*", payload.Data);*/
					//swap numbers
					//butnum[numptr[0]], butnum[i] = butnum[i], butnum[numptr[0]];
					butnum[i] = butnum[0];
				}
				ImGui::EndDragDropTarget();
			}
		}
		
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

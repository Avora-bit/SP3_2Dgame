/**
 CGUI_Scene2D
 @brief A class which manages the GUI for Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#include "GUI_Scene2D.h"

#include <iostream>
using namespace std;
#include <sstream>

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::CGUI_Scene2D(void)
	: cSettings(NULL)
	, window_flags(0)
	, m_fProgressBar(0.0f)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, cPlayer2D(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::~CGUI_Scene2D(void)
{
	if (cInventoryManager)
	{
		cInventoryManager->Destroy();
		cInventoryManager = NULL;
	}

	if (cPlayer2D)
	{
		cPlayer2D->Destroy();
		cPlayer2D = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

/**
  @brief Initialise this instance
  */
bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//// Show the mouse pointer
	//glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Initialise the cInventoryManager
	cInventoryManager = CInventoryManager::GetInstance();
	cPlayer2D = CPlayer2D::GetInstance();

	// These variables are for IMGUI demo only
	show_demo_window = true;
	show_another_window = true;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	m_fProgressBar = 0.0f;

	il = CImageLoader::GetInstance();


	//for (int i = 0; i < 3; i++)
	//{
	//	hbcells[i].setitemID(cPlayer2D->getitemval(i));

	//	hbcells[i].settextureID(hbcells[i].getitemID());


	//	hbcells[i].setquantity(cPlayer2D->getitem(i).getquantity());
	//	//hbcells[i].loadimagebasedID(hbcells[i].getitemID(), il);
	//	//hbcells[i].Init(il);

	//}


	return true;
}

/**
 @brief Update this instance
 */
void CGUI_Scene2D::Update(const double dElapsedTime)
{
	
	for (int i = 0; i < 3; i++)
	{
		hbcells[i].setitemID(cPlayer2D->getitemval(i));
		hbcells[i].settextureID(hbcells[i].getitemID());
		hbcells[i].setquantity(cPlayer2D->getitem(i).getquantity());
		//cout << "ID is " << hbcells[i].gettextureID() << endl;
	}
	


	// Calculate the relative scale to our default windows width
	const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	const float relativeScale_y = cSettings->iWindowHeight / 600.0f;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create an invisible window which covers the entire OpenGL window
	ImGui::Begin("Invisible window", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	//ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
	ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));

	ImGui::SetWindowFontScale(1.5f * relativeScale_y);

	// Display the FPS
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %d", cFPSCounter->GetFrameRate());
	
	ImVec4 col = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	
	{
		// Render the inventory items
		//cInventoryItem = cInventoryManager->GetItem("Bouquet");
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));  // Set a background color
		//ImGuiWindowFlags inventoryWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		//	ImGuiWindowFlags_NoTitleBar |
		//	ImGuiWindowFlags_NoMove |
		//	ImGuiWindowFlags_NoResize |
		//	ImGuiWindowFlags_NoCollapse |
		//	ImGuiWindowFlags_NoScrollbar;
		//ImGui::Begin("Image", NULL, inventoryWindowFlags);
		//ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.75f, cSettings->iWindowHeight * 0.85f));
		//ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
		//ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		//	ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y),
		//	ImVec2(0, 1), ImVec2(1, 0));
		//ImGui::SameLine();
		//ImGui::SetWindowFontScale(1.5f * relativeScale_y);
		//ImGui::PushStyleColor(ImGuiCol_FrameBg, col);
		//ImGui::ProgressBar(cInventoryItem->GetCount() /
		//	(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f * relativeScale_x, 20.0f * relativeScale_y));
		//ImGui::PopStyleColor();
		//ImGui::TextColored(ImVec4(1, 1, 1, 1), "Bouquet: %d / %d",
		//	cInventoryItem->GetCount(), cInventoryItem->GetMaxCount());
		//ImGui::PopStyleColor();
		//ImGui::End();

		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));  // Set a background color
		//ImGuiWindowFlags objectivesWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		//	ImGuiWindowFlags_NoTitleBar |
		//	ImGuiWindowFlags_NoMove |
		//	ImGuiWindowFlags_NoResize |
		//	ImGuiWindowFlags_NoCollapse |
		//	ImGuiWindowFlags_NoScrollbar;
		//ImGui::Begin("Objectives", NULL, objectivesWindowFlags);
		//ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.69f, cSettings->iWindowHeight * 0.01f));
		//ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
		//ImGui::SameLine();
		//ImGui::SetWindowFontScale(1.3f * relativeScale_y);
		//ImGui::TextColored(ImVec4(1, 1, 1, 1), "Objectives:\n-Collect flower bouquets\n-Enter door\n-Give flower bouquets\nto Princess",
		//	cInventoryItem->GetCount(), cInventoryItem->GetMaxCount());
		//ImGui::PopStyleColor();
		//ImGui::End();

		/*
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()!
		// You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			// Create a window called "Preview IMGUI" and append into it.
			ImGui::Begin("Preview IMGUI", NULL);
			ImGui::SetWindowPos(ImVec2(10.0f, 10.0f));				// Set the top-left of the window at (10,10)
			ImGui::SetWindowSize(ImVec2(300.0f, 200.0f));			// Set the window size to (300,200)

			ImGui::Text("Toggle IMGUI demo.");						// Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
		*/
	}




	//RENDER HEALTH BAR
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));  // Set a background color
		ImGuiWindowFlags healthWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin("Health", NULL, healthWindowFlags);
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.01f,
			cSettings->iWindowHeight * 0.03f));
		ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
		ImGui::SetWindowFontScale(1.5f * relativeScale_y);
		cInventoryItem = cInventoryManager->GetItem("Health");
		ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
			ImVec2(cInventoryItem->vec2Size.x * relativeScale_x,
				cInventoryItem->vec2Size.y * relativeScale_y),
			ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 0.6f));
		ImGui::ProgressBar(cInventoryItem->GetCount() /
			(float)cInventoryItem->GetMaxCount(), ImVec2(180.0f *
				relativeScale_x, 20.0f * relativeScale_y));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	//RENDER HUNGER BAR
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));  // Set a background color
		ImGuiWindowFlags hungerWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin("Hunger", NULL, hungerWindowFlags);
		/*ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.01f,
			cSettings->iWindowHeight * 0.03f));*/
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.01f, cSettings->iWindowHeight * 0.09f));
		ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));

		ImGui::SetWindowFontScale(1.5f * relativeScale_y);
		cInventoryItem = cInventoryManager->GetItem("Hunger");
		ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
			ImVec2(cInventoryItem->vec2Size.x * relativeScale_x,
				cInventoryItem->vec2Size.y * relativeScale_y),
			ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 1.0f, 0.0f, 0.6f));
		ImGui::ProgressBar(cInventoryItem->GetCount() /
			(float)cInventoryItem->GetMaxCount(), ImVec2(180.0f *
				relativeScale_x, 20.0f * relativeScale_y));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	//RENDER STAMINA BAR
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));  // Set a background color
		ImGuiWindowFlags staminaFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;

		/*ImGui::Begin("Hunger", NULL, staminaFlags);
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.01f, cSettings->iWindowHeight * 0.03f + 100));
		ImGui::SetWindowSize(ImVec2(180.0f * relativeScale_x, 25.0f * relativeScale_y));*/

		ImGui::Begin("Stamina", NULL, staminaFlags);
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.01f, cSettings->iWindowHeight * 0.15f));
		ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
		ImGui::SetWindowFontScale(1.5f * relativeScale_y);
		cInventoryItem = cInventoryManager->GetItem("Stamina");
		ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
			ImVec2(cInventoryItem->vec2Size.x* relativeScale_x,
				cInventoryItem->vec2Size.y* relativeScale_y),
			ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		
		ImGui::PushStyleColor(ImGuiCol_FrameBg, col);
		ImGui::ProgressBar(cInventoryItem->GetCount() /
			(float)cInventoryItem->GetMaxCount(), ImVec2(180.0f *
				relativeScale_x, 20.0f * relativeScale_y));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::End();
	}



	//DISPLAY WORDS
		// Create a window called "Hello, world!" and append into it.
	ImGui::Begin("QuantityText", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2((cSettings->iWindowWidth * 0.85f) / 2, cSettings->iWindowHeight * .85f));
	//ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
	ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));


	for (int n = 0; n < 3; n++)
	{
		ImGui::PushID(n);

		//don't break line if doesn't reach 3 cells
		if ((n % 3) != 0)
			ImGui::SameLine(0.f, 85.f);


		ImGui::SetWindowFontScale(2.f);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "% d", hbcells[n].getquantity()
		/*cInventoryItem->GetCount(), cInventoryItem->GetMaxCount()*/);

		ImGui::PopID();
	}
	ImGui::End();


	//RENDER HOTBAR
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));  // Set a background color
		ImGuiWindowFlags hotbarWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin("Hotbar", NULL, hotbarWindowFlags);
		ImGui::SetWindowPos(ImVec2((cSettings->iWindowWidth * 0.60f) / 2, cSettings->iWindowHeight * .85f));
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		for (int i = 0; i < 3; i++) {
			ImGui::PushID(i);
			ImGui::SameLine(0.f, 50.f);

			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (cPlayer2D->getActive(i) == true)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				if (ImGui::ImageButton((ImTextureID)hbcells[i].gettextureID(), ImVec2(cSettings->iWindowWidth * relativeScale_x * 0.05, cSettings->iWindowHeight * relativeScale_y * 0.05)))
				{
					for (int n = 0; n < 3; n++)
					{
						cPlayer2D->setActive(n, false);
						hbcells[n].setAct(cPlayer2D->getActive(n));
					}

					cPlayer2D->setActive(i, true);
					hbcells[i].setAct(cPlayer2D->getActive(i));
				}
				ImGui::PopStyleColor();
			}
			else
			{
				if (ImGui::ImageButton((ImTextureID)hbcells[i].gettextureID(), ImVec2(cSettings->iWindowWidth * relativeScale_x * 0.05, cSettings->iWindowHeight * relativeScale_y * 0.05)))
				{
					for (int n = 0; n < 3; n++)
					{
						cPlayer2D->setActive(n, false);
						hbcells[n].setAct(cPlayer2D->getActive(n));
					}

					cPlayer2D->setActive(i, true);
					hbcells[i].setAct(cPlayer2D->getActive(i));
				}
			}


			//if (hbcells[i].getAct() == true)
			//{
			//	//ImGui::IsItemActive();
			//	cout << "CURRENTLY ACTIVE IS " << i << endl;
			//}
			ImGui::PopStyleColor();
			ImGui::PopID();
		}
		ImGui::PopStyleColor();

		ImGui::End();

	}

	// projectile force
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));  // Set a background color
		ImGuiWindowFlags ProjectileForceFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;

		ImGui::Begin("Force", NULL, ProjectileForceFlags);
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.03f, cSettings->iWindowHeight * 0.9f));
		ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
		ImGui::SameLine();
		ImGui::SetWindowFontScale(1.5f * relativeScale_y);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, col);
		ImGui::ProgressBar((float)(cPlayer2D->getProjectileForce() / 7 * 100) /
			100, ImVec2(200.0f * relativeScale_x, 20.0f * relativeScale_y));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	/*ImGui::PopStyleColor();
	ImGui::End();*/

	//ImGui::PopStyleColor();
	ImGui::End();
}


/**
 @brief Set up the OpenGL display environment before rendering
 */
void CGUI_Scene2D::PreRender(void)
{
}

/**
 @brief Render this instance
 */
void CGUI_Scene2D::Render(void)
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	//render hotbar
	/*for (int i = 0; i < 3; i++)
	{
		hbcells[i].PreRender();
		hbcells[i].Render();
		hbcells[i].PostRender();

	}*/
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CGUI_Scene2D::PostRender(void)
{
}

int CGUI_Scene2D::return_hbcellid(int arr)
{
	return hbcells[arr].getitemID();
}

unsigned CGUI_Scene2D::return_textid(int arr)
{
	return hbcells[arr].gettextureID();
}

void CGUI_Scene2D::set_hbcellid(int arr, int itemid)
{
	hbcells[arr].setitemID(itemid);
	hbcells[arr].settextureID(hbcells[arr].getitemID());
	//hbcells[arr].loadimagebasedID(hbcells[arr].getitemID(), il);
}


//for (int i = 0; i < 3; i++)
//{
//	hbcells[i].setitemID(7);
//	hbcells[i].loadimagebasedID(hbcells[i].getitemID(), il);
//}
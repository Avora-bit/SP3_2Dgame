﻿#include "Camera.h"

#include <iostream>
Camera::Camera(void)
	:cKeyboardController(NULL),
	 cMouseController(NULL)
{
	vec2Index = glm::vec2(0.0f, 0.0f);
	zoom = 1.5f;
	mouseWeight = 0.15;
}

Camera::~Camera(void)
{
	cKeyboardController = NULL;
	cMouseController = NULL;
}

bool Camera::Init(void)
{
	cKeyboardController = CKeyboardController::GetInstance();
	cMouseController = CMouseController::GetInstance();

	return true;
}

bool Camera::Reset(void)
{
	return false;
}

void Camera::Update(const double dElapsedTime, glm::vec2 playerPos)
{
	float bounds = (zoom - 1) / zoom;
	glm::vec2 mousePos = glm::vec2(cMouseController->GetMousePositionX()-CSettings::GetInstance()->iWindowWidth/2, cMouseController->GetMousePositionY()-CSettings::GetInstance()->iWindowHeight/2); // mouse pos relative to center of window

	/* //arrow key control camera
	if (cKeyboardController->IsKeyDown(GLFW_KEY_UP) && vec2Index.y > -bounds)
	{
		vec2Index.y -= 0.015;
	}
	//else if (cKeyboardController->IsKeyDown(GLFW_KEY_UP))
	//	vec2Index.y = -0.24 * zoom;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_DOWN) && vec2Index.y < bounds)
	{
		vec2Index.y += 0.015;
	}
	//else if (cKeyboardController->IsKeyDown(GLFW_KEY_DOWN))
	//	vec2Index.y = 0.24 * zoom;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_LEFT) && vec2Index.x < bounds)
	{
		vec2Index.x += 0.015;
	}
	//else if (cKeyboardController->IsKeyDown(GLFW_KEY_LEFT))
	//	vec2Index.x = 0.24 * zoom;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_RIGHT) && vec2Index.x > -bounds)
	{
		vec2Index.x -= 0.015;
	}
	//else if (cKeyboardController->IsKeyDown(GLFW_KEY_RIGHT))
	//	vec2Index.x = -0.24 * zoom;
	*/
	//float newPosX = playerPos.x - cSettings->NUM_TILES_XAXIS / 2;
	//float newPosY = playerPos.y - cSettings->NUM_TILES_YAXIS / 2;
	//vec2Index = glm::vec2(-newPosX*(bounds*2/ cSettings->NUM_TILES_XAXIS), -newPosY*(bounds*2/cSettings->NUM_TILES_YAXIS));


	float newPosX = playerPos.x; // camera pos in vec2index
	float newPosY = playerPos.y;
	//if (mousePos.x < CSettings::GetInstance()->iWindowWidth/2 && mousePos.x > -((float)CSettings::GetInstance()->iWindowWidth/2) &&
	//	mousePos.y < CSettings::GetInstance()->iWindowHeight/2 && mousePos.y > -((float)CSettings::GetInstance()->iWindowHeight/2)) // within window
	{
		newPosX += mousePos.x / CSettings::GetInstance()->NUM_TILES_XAXIS * mouseWeight;
		newPosY -= mousePos.y / CSettings::GetInstance()->NUM_TILES_YAXIS * mouseWeight;
	}
	vec2Index = glm::vec2(1-newPosX/(CSettings::GetInstance()->NUM_TILES_XAXIS/2), 1-newPosY / (CSettings::GetInstance()->NUM_TILES_YAXIS/2));
	//std::cout << vec2Index.x << " " << vec2Index.y << std::endl;
	
	if (vec2Index.y < -bounds)
		vec2Index.y = -bounds;
	if (vec2Index.y > bounds)
		vec2Index.y = bounds;
	if (vec2Index.x > bounds)
		vec2Index.x = bounds;
	if (vec2Index.x < -bounds)
		vec2Index.x = -bounds;
}

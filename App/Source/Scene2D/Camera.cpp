#include "Camera.h"

#include <iostream>
Camera::Camera(void)
	:cKeyboardController(NULL),
	 cSettings(NULL)
{
	vec2Index = glm::vec2(0.0f, 0.0f);
	zoom = 1.5f;
}

Camera::~Camera(void)
{
	cKeyboardController = NULL;
}

bool Camera::Init(void)
{
	cKeyboardController = CKeyboardController::GetInstance();
	cSettings = CSettings::GetInstance();

	return true;
}

bool Camera::Reset(void)
{
	return false;
}

void Camera::Update(const double dElapsedTime, glm::vec2 playerPos)
{
	float bounds = (zoom - 1) / zoom;

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

	float newPosX = playerPos.x - cSettings->NUM_TILES_XAXIS / 2;
	float newPosY = playerPos.y - cSettings->NUM_TILES_YAXIS / 2;
	vec2Index = glm::vec2(-newPosX*(bounds*2/ cSettings->NUM_TILES_XAXIS), -newPosY*(bounds*2/cSettings->NUM_TILES_YAXIS));
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

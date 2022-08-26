#include "Camera.h"

#include <iostream>
Camera::Camera(void)
	:cKeyboardController(NULL),
	 cMouseController(NULL)
{
	vec2Index = glm::vec2(0.0f, 0.0f);
	pureVec2Index = glm::vec2(0.0f, 0.0f);
	zoom = 7.f;
	mouseWeight = 0.1f;
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
	glm::vec2 mousePos = glm::vec2(cMouseController->GetMousePositionX() - CSettings::GetInstance()->iWindowWidth / 2,
		cMouseController->GetMousePositionY() - CSettings::GetInstance()->iWindowHeight / 2); // mouse pos relative to center of window

	float newPosX = playerPos.x + 2 / CSettings::GetInstance()->NUM_STEPS_PER_TILE_XAXIS; // central origin of the player tile
	float newPosY = playerPos.y + 2 / CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS;

	pureVec2Index = glm::vec2(1 - newPosX / (CSettings::GetInstance()->NUM_TILES_XAXIS / 2),  // UV coord of camera centralised on player
							  1 - newPosY / (CSettings::GetInstance()->NUM_TILES_YAXIS / 2));
	{
		//update position with mouse
		newPosX += mousePos.x / CSettings::GetInstance()->NUM_TILES_XAXIS * mouseWeight;
		newPosY -= mousePos.y / CSettings::GetInstance()->NUM_TILES_YAXIS * mouseWeight;
	}
	vec2Index = glm::vec2(1 - newPosX / (CSettings::GetInstance()->NUM_TILES_XAXIS / 2), 
						  1 - newPosY / (CSettings::GetInstance()->NUM_TILES_YAXIS / 2));
	//camera new position with weight of the mouse
	if (vec2Index.y < -bounds) vec2Index.y = -bounds;
	if (vec2Index.y > bounds) vec2Index.y = bounds;
	if (vec2Index.x > bounds) vec2Index.x = bounds;
	if (vec2Index.x < -bounds) vec2Index.x = -bounds;

	//std::cout << vec2Index.x - pureVec2Index.x << " & " << vec2Index.y - pureVec2Index.y << std::endl;
	
	playerOffset = glm::vec2(cMouseController->GetMousePositionX() - CSettings::GetInstance()->iWindowWidth / 2 * (1 + (vec2Index.x - pureVec2Index.x) * zoom), 
		cMouseController->GetMousePositionY() - CSettings::GetInstance()->iWindowHeight / 2 * (1 - (vec2Index.y - pureVec2Index.y) * zoom));

}

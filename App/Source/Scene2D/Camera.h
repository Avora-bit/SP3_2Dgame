#pragma once
// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "Inputs\KeyboardController.h"
#include "Inputs\MouseController.h"

#include "GameControl\Settings.h"

class Camera: public CSingletonTemplate<Camera>
{
	friend CSingletonTemplate<Camera>;
public:
	bool Init(void);

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime, glm::vec2 playerPos);

	glm::vec2 vec2Index;
	glm::vec2 pureVec2Index;
	float zoom;

	glm::vec2 playerOffset;
protected:
	// Constructor
	Camera(void);
	// Destructor
	virtual ~Camera(void);

	CKeyboardController* cKeyboardController;
	CMouseController* cMouseController;
	float mouseWeight;
};

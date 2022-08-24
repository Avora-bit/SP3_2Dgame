#include "Primitives/Entity2D.h"    
#include "Player2D.h"
#include "Map2D.h"


// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include Settings
#include <string>


using namespace std;

#pragma once
class CItems2D :
    public CEntity2D
{
public:
	// Constructor
	CItems2D(void);

	// Destructor
	virtual ~CItems2D(void);

	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

protected:
	

	glm::vec2 vec2OldIndex;
	// Handler to the CMap2D instance
	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// The vec2 which stores the indices of the enemy2D in the Map2D
	glm::vec2 vec2Index;

	// The vec2 variable which stores the UV coordinates to render the enemy2D
	glm::vec2 vec2UVCoordinate;
	// Settings
	CSettings* cSettings;
	// Current color
	glm::vec4 runtimeColour;
	CSpriteAnimation* animatedSprites;
	CPlayer2D* cPlayer2D;
	CMap2D* cMap2D;
	Camera* camera;




	// Let enemy2D interact with the player
	bool InteractWithPlayer(void);


};




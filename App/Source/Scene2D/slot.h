#pragma once


//// Include GLEW
//#ifndef GLEW_STATIC
//#include <GL/glew.h>
//#define GLEW_STATIC
//#endif
//// Include GLM
//#include <includes/glm.hpp>
//#include <includes/gtc/matrix_transform.hpp>
//#include <includes/gtc/type_ptr.hpp>









#include <string>
#include "System\ImageLoader.h"

#include <map>
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include Settings
#include "GameControl\Settings.h"


#include "InventoryItem.h"
#include "InventoryManager.h"


using namespace std;
#include <iostream>
class slot 
{
private:
	string fileName;
	unsigned int textureID;
	unsigned int itemID;

	int quantity;


	CInventoryManager* cInventoryManager;
	CInventoryItem* cInventoryItem;




public:
	slot();
	~slot();

	int getitemID();
	unsigned gettextureID();
	void settextureID(int itemid);
	void setitemID(int iid);
};


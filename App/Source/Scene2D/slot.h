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

#include "Hilt2D.h"
#include "PlatinumHilt2D.h"
#include "WoodenHilt2D.h"
#include "IronHilt2D.h"

#include "Blade2D.h"
#include "KatanaBlade2D.h"
#include "DaggerBlade2D.h"
#include "CleaverBlade2D.h"
#include "RustyBlade2D.h"

#include "Sword2D.h"


class slot 
{
private:
	string fileName;
	unsigned int textureID;
	unsigned int itemID;

	int quantity;


	CInventoryManager* cInventoryManager;
	CInventoryItem* cInventoryItem;

	bool active;

	CHilt2D* hilt;
	CBlade2D* blade;
	CSword2D* sword;

public:
	slot();
	~slot();


	void AddQuantity(int amt);
	void SubtractQuantity(int amt);

	int getquantity(void);
	void setquantity(int amt);


	int getitemID(void);
	unsigned gettextureID(void);
	void settextureID(int itemid);

	void setAct(bool act);
	bool getAct();

	CHilt2D* returnHilt();
	CBlade2D* returnBlade();
	CSword2D* returnSword();
	


    void setSword(CSword2D* sw);
	void setHilt(int itemid);
	void setBlade(int itemid);


	void setitemID(int iid);
};


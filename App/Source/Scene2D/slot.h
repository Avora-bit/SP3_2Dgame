#pragma once



using namespace std;
#include <string>
#include "System\ImageLoader.h"


class slot
{

private:
	string fileName;
	unsigned textureID;
	unsigned int itemID;



public:
	slot();
	~slot();



	void loadimagebasedID(int itemid, CImageLoader* il);
	int getitemID();
	unsigned gettextureID();
	void setitemID(int iid);
	
};


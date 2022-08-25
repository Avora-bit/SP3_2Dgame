#include "slot.h"


slot::slot()
{
}

void slot::loadimagebasedID(int itemid, CImageLoader* il)
{
	/*switch (itemid)
	{
	case 1:
		fileName = "Image\\Sp3Images\\Base\\stick.png";
		break;
	case 2:
		fileName = "Image\\Sp3Images\\Base\\wood.png";
		break;
	case 0:
		fileName = "Image\\Sp3Images\\blank_output.png";
		break;
	case 6:
		fileName = "Image\\Sp3Images\\Weapons\\sword.png";
		break;
	case 7:
		fileName = "Image\\Sp3Images\\Weapons\\bow.png";
		break;
	}

	textureID = il->LoadTextureGetID(fileName.c_str(), false);*/
}

int slot::getitemID()
{
	return itemID;
}

unsigned slot::gettextureID()
{
	return textureID;
}

void slot::setitemID(int iid)
{
	itemID = iid;

}

slot::~slot()
{
	/*delete il;
	il = nullptr;*/
}

#include "slot.h"




slot::slot()
	//runtimeColour(glm::vec4(1.0f))
{
	
	/*glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);*/

	//cSettings = CSettings::GetInstance();
	//quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);


	cInventoryManager = CInventoryManager::GetInstance();
	//cInventoryItem= CInventoryItem::GetIns
}


int slot::getitemID()
{
	return itemID;
}

unsigned slot::gettextureID()
{
	return textureID;
}

void slot::settextureID(int itemid)
{
	//inventorySlots[i].settextureID(cInventoryManager->GetItem("Stick")->GetTextureID());

	switch (itemid)
	{
	case 1:
		textureID = cInventoryManager->GetItem("Stick")->GetTextureID();
		break;
	case 2:
		textureID = cInventoryManager->GetItem("Wood")->GetTextureID();
		break;
	case 7:
		textureID = cInventoryManager->GetItem("Swords")->GetTextureID();
		break;
	case 0:
		textureID = 0;
		break;
	default:
		break;
	}

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

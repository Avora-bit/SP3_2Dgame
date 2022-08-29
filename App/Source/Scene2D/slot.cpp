#include "slot.h"




slot::slot()
	//runtimeColour(glm::vec4(1.0f))
{
	quantity = 0;
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
	case 30:
		textureID = cInventoryManager->GetItem("Stick")->GetTextureID();
		break;
	case 40:
		textureID = cInventoryManager->GetItem("Wood")->GetTextureID();
		break;
	case 60:
		textureID = cInventoryManager->GetItem("Swords")->GetTextureID();
		break;
	case 102:
		textureID = cInventoryManager->GetItem("Campfire")->GetTextureID();
		break;
	case 70:
		textureID = cInventoryManager->GetItem("Raw Food")->GetTextureID();
		break;
	case 81:
		textureID = cInventoryManager->GetItem("Cooked Food")->GetTextureID();
		break;
	case 50:
		textureID = cInventoryManager->GetItem("Swords")->GetTextureID();
		break;
	case 0:
		textureID = 0;
		break;
	default:
		break;
	}

}

void slot::AddQuantity(int amt)
{

	quantity += amt;
}

void slot::SubtractQuantity(int amt)
{

	quantity -= amt;
}





void slot::setitemID(int iid)
{


	itemID = iid;

}

int slot::getquantity()
{
	return quantity;
}

void slot::setquantity(int amt)
{
	quantity = amt;
}


slot::~slot()
{

	/*delete il;
	il = nullptr;*/
}

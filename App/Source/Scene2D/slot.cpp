#include "slot.h"




slot::slot()
	//runtimeColour(glm::vec4(1.0f))
{
	quantity = 0;
	/*glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);*/

	//cSettings = CSettings::GetInstance();
	//quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	
	active = false;
	cInventoryManager = CInventoryManager::GetInstance();



	hilt = nullptr;
	blade = nullptr;
	sword = nullptr;
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
	//HILTS
	case 39:
		textureID = cInventoryManager->GetItem("pHilt")->GetTextureID();
		//hilt = new CPlatinumHilt2D();
		break;
	case 38:
		textureID = cInventoryManager->GetItem("wHilt")->GetTextureID();
		//hilt = new CWoodenHilt2D();
		break;
	case 37:
		textureID = cInventoryManager->GetItem("iHilt")->GetTextureID();
		//hilt = new CIronHilt2D();
		break;
	//
	//BLADES
	case 36:
		textureID = cInventoryManager->GetItem("rBlade")->GetTextureID();
		//blade = new CRustyBlade2D();
		break;
	case 35:
		textureID = cInventoryManager->GetItem("dBlade")->GetTextureID();
		//blade = new CDaggerBlade2D();
		break;
	case 34:
		textureID = cInventoryManager->GetItem("cBlade")->GetTextureID();
		//blade = new CCleaverBlade2D();
		break;
	case 33:
		textureID = cInventoryManager->GetItem("kBlade")->GetTextureID();
		//blade = new CKatanaBlade2D();
		break;
	//
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
	case 49:
		textureID = cInventoryManager->GetItem("Rock")->GetTextureID();
		break;
	case 90:
		textureID = cInventoryManager->GetItem("Shivs")->GetTextureID();
		break;
	case 89:
		textureID = cInventoryManager->GetItem("Shovel")->GetTextureID();
		break;
	case 0:
		textureID = 0;
		break;
	default:
		break;
	}

}

void slot::setAct(bool act)
{
	active = act;
}

bool slot::getAct()
{
	return active;
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

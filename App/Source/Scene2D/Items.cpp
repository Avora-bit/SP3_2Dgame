#include "Items.h"

#include "RenderControl/ShaderManager.h"


CItems2D::CItems2D(void)
{
}

CItems2D::~CItems2D(void)
{
}

bool CItems2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();


	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	

	cPlayer2D = CPlayer2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;


	if (cMap2D->FindValue(1, uiRow, uiCol) == false
		|| cMap2D->FindValue(2, uiRow, uiCol) == false
		|| cMap2D->FindValue(7, uiRow, uiCol) == false)
	{
		return false;	// Unable to find the start position of the player, so quit this game
	}

	// Erase the value of the item in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	//quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);


	switch (cMap2D->GetMapInfo(uiCol, uiRow))
	{
		case 1:
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/stick.png", true);
			break;
		case 2:
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/wood.png", true);
			break;
		default:
			break;
		
	}
	if (iTextureID == 0)
	{
		std::cout << "Failed to load enemy tile texture" << std::endl;
		return false;
	}

	/*animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 2, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idleLeft", 1, 1);
	animatedSprites->AddAnimation("idleRight", 0, 0);

	animatedSprites->PlayAnimation("idleRight", -1, 0.3f);*/

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);


	return true;
}

void CItems2D::Update(const double dElapsedTime)
{
	
	
}

void CItems2D::PreRender(void)
{
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use("Shader2D");
}

void CItems2D::Render(void)
{
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
		vec2UVCoordinate.y,
		0.0f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// Get the texture to be rendered
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	glBindVertexArray(VAO);

	//CS: Use mesh to render
	mesh->Render();

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CItems2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

bool CItems2D::InteractWithPlayer(void)
{
	glm::vec2 vec2PlayerPos = cPlayer2D->vec2Index;

	// Check if the enemy2D is within 1.5 indices of the player2D
	if (((vec2Index.x >= vec2PlayerPos.x - 0.5) &&
		(vec2Index.x <= vec2PlayerPos.x + 0.5))
		&&
		((vec2Index.y >= vec2PlayerPos.y - 0.5) &&
			(vec2Index.y <= vec2PlayerPos.y + 0.5)))
	{
		
	}
	return false;
}

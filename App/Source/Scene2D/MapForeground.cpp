/**
 Map2D
 @brief A class which manages the map in the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "MapForeground.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include Filesystem
#include "System\filesystem.h"
// Include ImageLoader
#include "System\ImageLoader.h"
#include "Primitives/MeshBuilder.h"

using namespace std;

// For AStar PathFinding
using namespace std::placeholders;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
MapForeground::MapForeground(void)
	: uiCurLevel(0)
	, quadMesh(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
MapForeground::~MapForeground(void)
{
	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = NULL;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// Set this to NULL since it was created elsewhere, so we let it be deleted there.
	cSettings = NULL;

	camera = NULL;
}

/**
@brief Init Initialise this instance
*/ 
bool MapForeground::Init(	const unsigned int uiNumLevels,
					const unsigned int uiNumRows,
					const unsigned int uiNumCols)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	camera = Camera::GetInstance();

	// Create the arrMapInfo and initialise to 0
	// Start by initialising the number of levels
	arrMapInfo = new Grid** [uiNumLevels];
	for (unsigned uiLevel = 0; uiLevel < uiNumLevels; uiLevel++)
	{
		arrMapInfo[uiLevel] = new Grid* [uiNumRows];
		for (unsigned uiRow = 0; uiRow < uiNumRows; uiRow++)
		{
			arrMapInfo[uiLevel][uiRow] = new Grid[uiNumCols];
			for (unsigned uiCol = 0; uiCol < uiNumCols; uiCol++)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
			}
		}
	}

	// Store the map sizes in cSettings
	uiCurLevel = 0;
	this->uiNumLevels = uiNumLevels;
	cSettings->NUM_TILES_XAXIS = uiNumCols;
	cSettings->NUM_TILES_YAXIS = uiNumRows;
	cSettings->UpdateSpecifications();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load and create textures
	//solid tiles
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/MapTiles/tree.tga", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/tree.tga" << endl;
			return false;
		}
		else
		{
			// Store the texture ID into MapOfTextureIDs
			MapOfTextureIDs.insert(pair<int, int>(100, iTextureID));
		}

		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/MapTiles/wall.tga", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/wall.tga" << endl;
			return false;
		}
		else
		{
			// Store the texture ID into MapOfTextureIDs
			MapOfTextureIDs.insert(pair<int, int>(101, iTextureID));
		}
	}

	//interactable items
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/MapEntities/treasure.tga", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/treasure.tga" << endl;
			return false;
		}
		else
		{
			// Store the texture ID into MapOfTextureIDs
			MapOfTextureIDs.insert(pair<int, int>(80, iTextureID));
		}
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/MapEntities/web.tga", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/web.tga" << endl;
			return false;
		}
		else
		{
			// Store the texture ID into MapOfTextureIDs
			MapOfTextureIDs.insert(pair<int, int>(79, iTextureID));
		}
	}
	
	//FOR INVENTORY TESTING PURPOSES - REAGAN
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/Base/stick.tga", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Sp3Images/Base/stick.tga" << endl;
		return false;
	}
	else
	{
		MapOfTextureIDs.insert(pair<int, int>(1, iTextureID));
	}


	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Sp3Images/Base/wood.tga", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Sp3Images/Base/wood.tga" << endl;
		return false;
	}
	else
	{
		MapOfTextureIDs.insert(pair<int, int>(2, iTextureID));
	}

	return true;
}

/**
@brief Update Update this instance
*/
void MapForeground::Update(const double dElapsedTime)
{
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void MapForeground::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render Render this instance
 */
void MapForeground::Render(void)
{
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// Render
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

			transform = glm::scale(transform, glm::vec3(camera->zoom, camera->zoom, 0));

			transform = glm::translate(transform, glm::vec3(cSettings->ConvertIndexToUVSpace(cSettings->x, uiCol, false, camera->vec2Index.x),
															cSettings->ConvertIndexToUVSpace(cSettings->y, uiRow, true, camera->vec2Index.y),
															0.0f));

			//transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

			// Update the shaders with the latest transform
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

			// Render a tile
			RenderTile(uiRow, uiCol);
		}
	}
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void MapForeground::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

// Set the specifications of the map
void MapForeground::SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "MapForeground::SetNumTiles() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_TILES_XAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_TILES_YAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

// Set the specifications of the map
void MapForeground::SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "MapForeground::SetNumSteps() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_STEPS_PER_TILE_XAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_STEPS_PER_TILE_YAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

/**
 @brief Set the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to set to
 @param iCol A const int variable containing the column index of the element to set to
 @param iValue A const int variable containing the value to assign to this arrMapInfo
 */
void MapForeground::SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert)
{
	if (bInvert)
		arrMapInfo[uiCurLevel][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value = iValue;
	else
		arrMapInfo[uiCurLevel][uiRow][uiCol].value = iValue;
}

/**
 @brief Get the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to get from
 @param iCol A const int variable containing the column index of the element to get from
 @param bInvert A const bool variable which indicates if the row information is inverted
 */
int MapForeground::GetMapInfo(const unsigned int uiRow, const int unsigned uiCol, const bool bInvert) const
{
	if (bInvert)
		return arrMapInfo[uiCurLevel][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value;
	else
		return arrMapInfo[uiCurLevel][uiRow][uiCol].value;
}

/**
 @brief Load a map
 */ 
bool MapForeground::LoadMap(string filename, const unsigned int uiCurLevel)
{
	doc = rapidcsv::Document(FileSystem::getPath(filename).c_str());

	//(*arrMapSizes).uiRowSize = (unsigned int)doc.GetRowCount();
	//(*arrMapSizes).uiColSize = (unsigned int)doc.GetColumnCount();

	// Check if the sizes of CSV data matches the declared arrMapInfo sizes
	if ((cSettings->NUM_TILES_XAXIS != (unsigned int)doc.GetColumnCount()) ||
		(cSettings->NUM_TILES_YAXIS != (unsigned int)doc.GetRowCount()))
	{
		if (cSettings->NUM_TILES_XAXIS != (unsigned int)doc.GetColumnCount()) {
			cout << (unsigned int)doc.GetColumnCount() << " != " << cSettings->NUM_TILES_XAXIS << endl;
			cout << "Column of CSV map does not match declared arrMapInfo column size." << endl;
		}
		if (cSettings->NUM_TILES_YAXIS != (unsigned int)doc.GetRowCount()) {
			cout << (unsigned int)doc.GetRowCount() << " != " << cSettings->NUM_TILES_YAXIS << endl;
			cout << "Row of CSV map does not match declared arrMapInfo row size." << endl;
		}

		
		return false;
	}

	// Read the rows and columns of CSV data into arrMapInfo
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		// Read a row from the CSV file
		std::vector<std::string> row = doc.GetRow<std::string>(uiRow);
		
		// Load a particular CSV value into the arrMapInfo
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
		{
			arrMapInfo[uiCurLevel][uiRow][uiCol].value = (int)stoi(row[uiCol]);
		}
	}

	return true;
}

/**
 @brief Save the tilemap to a text file
 @param filename A string variable containing the name of the text file to save the map to
 */
bool MapForeground::SaveMap(string filename, const unsigned int uiCurLevel)
{
	// Update the rapidcsv::Document from arrMapInfo
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			doc.SetCell(uiCol, uiRow, arrMapInfo[uiCurLevel][uiRow][uiCol].value);
		}
		cout << endl;
	}

	// Save the rapidcsv::Document to a file
	doc.Save(FileSystem::getPath(filename).c_str());

	return true;
}

/**
@brief Find the indices of a certain value in arrMapInfo
@param iValue A const int variable containing the row index of the found element
@param iRow A const int variable containing the row index of the found element
@param iCol A const int variable containing the column index of the found element
@param bInvert A const bool variable which indicates if the row information is inverted
*/
bool MapForeground::FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert)
{
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			if (arrMapInfo[uiCurLevel][uiRow][uiCol].value == iValue)
			{
				if (bInvert)
					uirRow = cSettings->NUM_TILES_YAXIS - uiRow - 1;
				else
					uirRow = uiRow;
				uirCol = uiCol;
				return true;	// Return true immediately if the value has been found
			}
		}
	}
	return false;
}

/**
 @brief Set current level
 */
void MapForeground::SetCurrentLevel(unsigned int uiCurLevel)
{
	if (uiCurLevel < uiNumLevels)
	{
		this->uiCurLevel = uiCurLevel;
	}
}
/**
 @brief Get current level
 */
unsigned int MapForeground::GetCurrentLevel(void) const
{
	return uiCurLevel;
}

/**
 @brief Render a tile at a position based on its tile index
 @param iRow A const int variable containing the row index of the tile
 @param iCol A const int variable containing the column index of the tile
 */
void MapForeground::RenderTile(const unsigned int uiRow, const unsigned int uiCol)
{
	if ((arrMapInfo[uiCurLevel][uiRow][uiCol].value > 0) &&
		(arrMapInfo[uiCurLevel][uiRow][uiCol].value < 200))
	{
		//if (arrMapInfo[uiCurLevel][uiRow][uiCol].value < 3)
 		glBindTexture(GL_TEXTURE_2D, MapOfTextureIDs.at(arrMapInfo[uiCurLevel][uiRow][uiCol].value));

		glBindVertexArray(VAO);
		//CS: Render the tile
		quadMesh->Render();
		glBindVertexArray(0);
	}
}

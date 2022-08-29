/**
 Map2D
 @brief A class which manages the map in the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include the RapidCSV
#include "System/rapidcsv.h"
// Include map storage
#include <map>

// Include files for AStar
#include <queue>
#include <functional>

// Include Settings
#include "GameControl\Settings.h"

// Include Entity2D
#include "Primitives/Entity2D.h"

#include "Camera.h"


struct MapSize {
	unsigned int uiRowSize;
	unsigned int uiColSize;
};

// A structure storing information about a map grid
// It includes data to be used for A* Path Finding
struct Grid {
	unsigned int value;		//id of tile
	glm::vec2 pos;			//xy coord of tile
	glm::vec2 parent;		//a*
	unsigned int f;			//a*
	unsigned int g;			//a*
	unsigned int h;			//a*
	Grid()
		: value(0), pos(0, 0), parent(-1, -1), f(0), g(0), h(0) {}
	Grid(const glm::vec2& pos, unsigned int f)
		: value(0), pos(pos), parent(-1, 1), f(f), g(0), h(0) {}
	Grid(const glm::vec2& pos, const glm::vec2& parent,
		unsigned int f, unsigned int g, unsigned int h)
		: value(0), pos(pos), parent(parent), f(f), g(g), h(h) {}
};

// A structure storing information about Map Sizes
using HeuristicFunction = std::function<unsigned int(const glm::vec2&, const glm::vec2&, int)>;
// Reverse std::priority_queue to get the smallest element on top
inline bool operator< (const Grid& a, const Grid& b) { return b.f < a.f; }

namespace heuristic
{
	unsigned int manhattan(const glm::vec2& v1, const glm::vec2& v2, int weight);
	unsigned int euclidean(const glm::vec2& v1, const glm::vec2& v2, int weight);
}

class CMap2D : public CSingletonTemplate<CMap2D>, public CEntity2D
{
	friend CSingletonTemplate<CMap2D>;
public:
	// Init
	bool Init(	const unsigned int uiNumLevels = 1,
				const unsigned int uiNumRows = 24,
				const unsigned int uiNumCols = 32);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Set the specifications of the map
	void SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue);
	void SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue);

	// Set the value at certain indices in the arrMapInfo
	void SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert = true, const int uilayer = 1);

	void SetMapInfo_2(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert, const unsigned int uilayer);


	// Get the value at certain indices in the arrMapInfo
	int GetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert = true, const int uilayer = 1);		//0 is background, 1 if foreground

	// Load a map
	bool LoadMap(string BGfilename, string FGfilename, const unsigned int uiLevel = 0);
	
	// Find the indices of a certain value in arrMapInfo
	bool FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert = true, const int uilayer = 1);

	// Set current level
	void SetCurrentLevel(unsigned int uiCurLevel);
	// Get current level
	unsigned int GetCurrentLevel(void) const;

	// For AStar PathFinding
	std::vector<glm::vec2> PathFind(const glm::vec2& startPos, const glm::vec2& targetPos, HeuristicFunction heuristicFunc, int weight = 1);
	// Set if AStar PathFinding will consider diagonal movements
	void SetDiagonalMovement(const bool bEnable);

	void PrintSelf(void) const;

protected:
	// The variable containing the rapidcsv::Document
	// We will load the CSV file's content into this Document
	rapidcsv::Document doc;

	// A 3-D array which stores the values of the tile map
	Grid**** arrMapInfo;

	// The current level
	unsigned int uiCurLevel;
	// The number of levels
	unsigned int uiNumLevels;

	glm::vec2 cameraStart;

	Camera* camera;

	// A 1-D array which stores the map sizes for each level
	MapSize* arrMapSizes;

	// Map containing texture IDs
	map<int, int> MapOfTextureIDs;

	//CS: The quadMesh for drawing the tiles
	CMesh* quadMesh;

	// Constructor
	CMap2D(void);

	// Destructor
	virtual ~CMap2D(void);

	// Render a tile
	void RenderTile(const unsigned int uiRow, const unsigned int uiCol, int layer);

	// For A-Star PathFinding
	// Build a path from m_cameFromList after calling PathFind()
	std::vector<glm::vec2> BuildPath() const;
	// Check if a grid is valid
	bool isValid(const glm::vec2& pos) const;
	// Check if a grid is blocked
	bool isBlocked( const unsigned int uiRow,
					const unsigned int uiCol,
					const bool bInvert = true, 
					const int uilayer = 1);			//default layer is 1 cause foreground
	// Convert a position to a 1D position in the array
	int ConvertTo1D(const glm::vec2& pos) const;

	// Delete AStar lists
	bool DeleteAStarLists(void);
	// Reset AStar lists
	bool ResetAStarLists(void);

	int m_weight;
	//int m_size;
	unsigned int m_nrOfDirections;
	//glm::vec2 m_dimensions;
	glm::vec2 m_startPos;
	glm::vec2 m_targetPos;

	std::priority_queue<Grid> m_openList;
	std::vector<bool> m_closedList;
	std::vector<Grid> m_cameFromList;
	//std::vector<int> m_grid;
	std::vector<glm::vec2> m_directions;
	HeuristicFunction m_heuristic;
};


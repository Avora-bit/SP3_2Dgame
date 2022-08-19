/**
CHilt2D
 */
#pragma once

#include "InventoryItem.h"

class CHilt2D : public CInventoryItem
{
public:
	// Constructor
	CHilt2D(void);

	// Destructor
	virtual ~CHilt2D(void);

protected:
	float bonusMultiplier;
	unsigned short int light;
	unsigned short int ravenous;
	unsigned short int wide;
	unsigned short int sharp;

};



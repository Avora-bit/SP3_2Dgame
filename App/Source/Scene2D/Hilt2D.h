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

	float getBonusMultiplier();
	unsigned short int getLight();
	unsigned short int getRavenous();
	unsigned short int getWide();
	unsigned short int getSharp();

	void addLight(int level);
	void addRavenous(int level);
	void addWide(int level);
	void addSharp(int level);
protected:
	float bonusMultiplier;
	unsigned short int light;
	unsigned short int ravenous;
	unsigned short int wide;
	unsigned short int sharp;

};



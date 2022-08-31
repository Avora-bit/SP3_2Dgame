#pragma once
#include "../Scene2D/Sword2D.h"

/**
 CGameStateBase
 @brief This is an Abstract Class. Derive from this class to implement it.
 By: Toh Da Jun
 Date: July 2021
 */
class CGameStateBase
{
public:
	// Constructor
	CGameStateBase(void) {}
	// Destructor
	virtual ~CGameStateBase(void) {}

	// These are pure virtual functions
	virtual bool Init() = 0;
	virtual bool Update(const double dElapsedTime) = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;


	virtual int returnbutnumval(int arr) {
		return arr;

	};

	virtual void setbutnumvalto(int arr, int val) {};



	virtual void setquantity(int arr, int quantity) {

	}
	virtual int return_butnumQuantity(int arr) {
		return arr;
	}

	
	

};

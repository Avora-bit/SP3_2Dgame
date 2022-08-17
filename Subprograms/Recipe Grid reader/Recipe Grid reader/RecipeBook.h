#pragma once
#include "Recipe.h"

#include <vector>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

//requires check from the inventory UI, index 0 is empty cause thats the expected output. 1-9 must be filled
class RecipeBook {
public:
	RecipeBook(string txtfilename);
	~RecipeBook();

	void CreateRecipe();

	int CheckRecipe(Recipe tempRecipe);

	void PrintBook();
private:
	string filename;
	vector<Recipe> recipeList;			//create empty vector
};


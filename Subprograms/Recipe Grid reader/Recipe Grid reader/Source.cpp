#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

#include "Recipe.h"
#include "RecipeBook.h"


int main(void) {
	//text file reader for minecraft like recipes

	RecipeBook* recipebook = new RecipeBook("Recipes.txt");
	recipebook->CreateRecipe();

	recipebook->PrintBook();
	Recipe gridrecipe;


	int input;

	//column zero is always 0
	gridrecipe.SetRecipeIndex(0, 0);

	gridrecipe.SetRecipeIndex(1, 2);
	gridrecipe.SetRecipeIndex(2, 1);
	gridrecipe.SetRecipeIndex(3, 1);
	gridrecipe.SetRecipeIndex(4, 2);
	gridrecipe.SetRecipeIndex(5, 1);
	gridrecipe.SetRecipeIndex(6, 1);
	gridrecipe.SetRecipeIndex(7, 1);
	gridrecipe.SetRecipeIndex(8, 2);
	gridrecipe.SetRecipeIndex(9, 2);
	//hardcode value for recipe
	std::cout << recipebook->CheckRecipe(gridrecipe) << std::endl;


	//set the ingredients required

	for (int i = 1; i < 10; i++)
	{
		cout << "Input a value" << endl;
		cin >> input;
		gridrecipe.SetRecipeIndex(i, input);
	}

	/*gridrecipe.SetRecipeIndex(1, 2);
	gridrecipe.SetRecipeIndex(2, 1);
	gridrecipe.SetRecipeIndex(3, 1);
	gridrecipe.SetRecipeIndex(4, 2);
	gridrecipe.SetRecipeIndex(5, 1);
	gridrecipe.SetRecipeIndex(6, 1);
	gridrecipe.SetRecipeIndex(7, 1);
	gridrecipe.SetRecipeIndex(8, 2);
	gridrecipe.SetRecipeIndex(9, 4);*/
	//hardcode value for recipe
	std::cout << recipebook->CheckRecipe(gridrecipe) << std::endl;

	//create large vector for all recipes
	//create temporary recipe class
	//fill recipe with recipe from crafting grid
	//compare temporary recipe with all recipes
	//get result of the comparison and check if its true


	//delete all new arrays
	delete recipebook;

	return 0;
}
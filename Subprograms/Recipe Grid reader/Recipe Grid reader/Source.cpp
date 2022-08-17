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

	//column zero is always 0
	gridrecipe.SetRecipeIndex(0, 0);

	//set the ingredients required
	gridrecipe.SetRecipeIndex(1, 2);
	gridrecipe.SetRecipeIndex(2, 2);
	gridrecipe.SetRecipeIndex(3, 2);
	gridrecipe.SetRecipeIndex(4, 2);
	gridrecipe.SetRecipeIndex(5, 2);
	gridrecipe.SetRecipeIndex(6, 2);
	gridrecipe.SetRecipeIndex(7, 2);
	gridrecipe.SetRecipeIndex(8, 2);
	gridrecipe.SetRecipeIndex(9, 4);
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
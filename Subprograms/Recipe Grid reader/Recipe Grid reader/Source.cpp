#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

class Recipe{
public: 
	Recipe() {
	}
	~Recipe() {
	}
	void SetRecipeIndex(int index, int value) {
		lst_recipe[index] = value;
	}
	int GetRecipeIndex(int index) {
		return lst_recipe[index];
	}
	
private:
	int lst_recipe[10] = {};
	//0 = output id
	//1-9 = recipe pattern
	//1 2 3
	//4 5 6
	//7 8 9
};

//requires check from the inventory UI, index 0 is empty cause thats the expected output. 1-9 must be filled
class RecipeBook {
public:
	RecipeBook(string txtfilename) {
		filename = txtfilename;
	}
	~RecipeBook() {
		//cleaning
	}

	void CreateRecipe() {
		ifstream book(filename);		//input only
		//create array
		string linetext;	Recipe tempRecipe;			//create new recipe
		while (getline(book, linetext, ' ')) {		//space for carriage return, hardcoded ',' for delimiter
			// Output the text from the file
			int value, count = 0;	string token;  size_t pos;
			while ((pos = linetext.find(',')) != std::string::npos) {
				token = linetext.substr(0, pos);
				value = stoi(token);
				tempRecipe.SetRecipeIndex(count, value);
				count++;
				linetext.erase(0, pos + 1);		//delete index
			}
			count = 0;
			//push recipe
			recipeList.push_back(tempRecipe);
		}
		book.close();
	}

	int CheckRecipe(Recipe tempRecipe) {
		int output;		//output item index
		for (std::vector<Recipe>::iterator it = recipeList.begin(); it != recipeList.end(); ++it) {
			bool check = true;
			for (int i = 1; i < 10; i++) {
				if (tempRecipe.GetRecipeIndex(i) != it->GetRecipeIndex(i)) {
<<<<<<< Updated upstream
=======
					std::cout << tempRecipe.GetRecipeIndex(i) << ", " << it->GetRecipeIndex(i) << std::endl;
>>>>>>> Stashed changes
					check = false;
					break;
				}
			}
			if (check) {
				output = it->GetRecipeIndex(0);
				return output;
			}
		}
		return 0;
	}

	void PrintBook(void) {
		for (std::vector<Recipe>::iterator it = recipeList.begin(); it != recipeList.end(); ++it) {
			for (int i = 0; i < 10; i++) {
					std::cout << it->GetRecipeIndex(i) << ", ";
			}
			std::cout << " " << std::endl;

		}
	}
private:
	string filename;
	vector<Recipe> recipeList;			//create empty vector
};

int main(void) {
	//text file reader for minecraft like recipes

	RecipeBook* recipebook = new RecipeBook("Recipes.txt");
	recipebook->CreateRecipe();

	recipebook->PrintBook();
	Recipe gridrecipe;
	gridrecipe.SetRecipeIndex(0, 0);
	gridrecipe.SetRecipeIndex(1, 1);
	gridrecipe.SetRecipeIndex(2, 1);
	gridrecipe.SetRecipeIndex(3, 1);
	gridrecipe.SetRecipeIndex(4, 1);
	gridrecipe.SetRecipeIndex(5, 1);
	gridrecipe.SetRecipeIndex(6, 1);
	gridrecipe.SetRecipeIndex(7, 1);
	gridrecipe.SetRecipeIndex(8, 1);
	gridrecipe.SetRecipeIndex(9, 3);
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
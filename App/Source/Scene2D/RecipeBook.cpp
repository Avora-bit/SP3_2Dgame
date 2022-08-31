#include "RecipeBook.h"

RecipeBook::RecipeBook(string txtfilename) {
	filename = txtfilename;
}

RecipeBook::~RecipeBook() {
	//cleaning
}

void RecipeBook::CreateRecipe() {
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

int RecipeBook::CheckRecipe(Recipe tempRecipe) {
	int output;		//output item index
	for (std::vector<Recipe>::iterator it = recipeList.begin(); it != recipeList.end(); ++it) {
		bool check = true;
		for (int i = 1; i < 10; i++) {
			if (tempRecipe.GetRecipeIndex(i) != it->GetRecipeIndex(i)) {
				/*std::cout << tempRecipe.GetRecipeIndex(i) << ", " << it->GetRecipeIndex(i) << std::endl;*/
				check = false;
				break;
			}
		}
		if (check) {
			//print out the value stored in the index
			output = it->GetRecipeIndex(0);
			return output;
		}
	}
	return 0;
}

void RecipeBook::PrintBook(void) {
	for (std::vector<Recipe>::iterator it = recipeList.begin(); it != recipeList.end(); ++it) {
		for (int i = 0; i < 10; i++) {
			std::cout << it->GetRecipeIndex(i) << ", ";
		}
		std::cout << " " << std::endl;

	}
}
#pragma once



class Recipe {
public:
	Recipe();
	~Recipe();
	void SetRecipeIndex(int index, int value);
	int GetRecipeIndex(int index);

private:
	int lst_recipe[10] = {};
	//0 = output id
	//1-9 = recipe pattern
	//1 2 3
	//4 5 6
	//7 8 9
};


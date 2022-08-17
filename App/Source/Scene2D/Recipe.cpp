#include "Recipe.h"

Recipe::Recipe() {
}
Recipe::~Recipe() {
}
void Recipe::SetRecipeIndex(int index, int value) {
	lst_recipe[index] = value;
}
int Recipe::GetRecipeIndex(int index) {
	return lst_recipe[index];
}
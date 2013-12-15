#include "search_bar_model.hpp"

SearchBarModel::SearchBarModel()
{
}

void SearchBarModel::set_data(std::string new_expected, size_t new_pivot)
{
	expected = new_expected;
	pivot = new_pivot;
}

std::string& SearchBarModel::get_expected()
{
	return expected;
}

size_t& SearchBarModel::get_pivot()
{
	return pivot;
}

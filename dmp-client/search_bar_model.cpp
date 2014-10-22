#include "search_bar_model.hpp"

SearchBarModel::SearchBarModel()
{
}

void SearchBarModel::set_query(std::string new_query)
{
	query = new_query;
}

void SearchBarModel::set_data(std::string new_expected, size_t new_pivot)
{
	expected = new_expected;
	pivot = new_pivot;
	call_on_delegates<SearchBarUiDelegate>(&SearchBarUiDelegate::query_parse_error);
}

std::string SearchBarModel::get_expected() const
{
	return expected;
}

size_t SearchBarModel::get_pivot() const
{
	return pivot;
}

size_t SearchBarModel::get_query_length() const
{
	return query.size();
}

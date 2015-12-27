#include "search_bar_model.hpp"

#include "search_bar_ui_delegate.hpp"

SearchBarModel::SearchBarModel()
{
}

void SearchBarModel::set_query(std::string new_query)
{
	query = new_query;
}

void SearchBarModel::clear()
{
	call_on_delegates(&SearchBarUiDelegate::clear_start);
	query.clear();
	expected.clear();
	pivot = 0;
	call_on_delegates(&SearchBarUiDelegate::clear_end);
}

void SearchBarModel::set_data(std::string new_expected, size_t new_pivot)
{
	expected = new_expected;
	pivot = new_pivot;
	call_on_delegates(&SearchBarUiDelegate::query_parse_error);
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

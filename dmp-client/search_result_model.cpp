#include "search_result_model.hpp"

#include "search_result_ui_delegate.hpp"

#include "message.hpp"

#include "fusion_static_dispatch.hpp"

#include <string>
#include <stdexcept>

namespace dmp_library { struct LibraryEntry; }

SearchResultModel::SearchResultModel()
{
}

void SearchResultModel::add_search_response(message::SearchResponse response)
{
	int count = response.results.size();
	
	if(count != 0) {
		call_on_delegates(&SearchResultUiDelegate::search_results_start, count);
		search_results.push_back(std::make_pair(Client(response.origin), response.results));
		call_on_delegates(&SearchResultUiDelegate::search_results_end);
	}
}

int SearchResultModel::row_count() const
{
	int size = 0;
	for(auto&& results : search_results) {
		size += results.second.size();
	}
	
	return size;
}

int SearchResultModel::column_count() const
{
	return friendly_fusion::result_of::size<dmp_library::LibraryEntry>::type::value + 1;
}

boost::any SearchResultModel::get_cell(int row, int column) const
{
	if(column < 0 || size_t(column) >= number_of_library_entry_members + 1) {
		throw std::out_of_range("Column index was out of range.");
	}

	for(SearchResultsElement const& p : search_results)
	{
		if(row > 0 && size_t(row) >= p.second.size()) {
			row -= p.second.size();
		} else {
			return get_nth(boost::fusion::joint_view<dmp_library::LibraryEntry const, Client const>(p.second.at(row).second, p.first), column);
		}
	}

	throw std::out_of_range("Row index was out of range.");
}

std::string SearchResultModel::header_data(int section) const
{
	if(section < 0 || size_t(section) >= number_of_library_entry_members + 1) {
		throw std::out_of_range("Column index was out of range.");
	}
	
	return get_nth_name<boost::fusion::joint_view<dmp_library::LibraryEntry, Client>>(section);
}

void SearchResultModel::clear()
{
	call_on_delegates(&SearchResultUiDelegate::clear_start);
	search_results.clear();
	call_on_delegates(&SearchResultUiDelegate::clear_end);
}

void SearchResultModel::set_current_query(std::string query)
{
	call_on_delegates(&SearchResultUiDelegate::new_search_begin);
	clear();
	current_query = query;
	call_on_delegates(&SearchResultUiDelegate::new_search_end);
	
}

std::string SearchResultModel::get_current_query() const
{
	return current_query;
}

std::tuple<std::string, uint32_t, dmp_library::LibraryEntry> SearchResultModel::get_row_info(int row) const
{
	for(auto&& p : search_results)
	{
		if(row > 0 && size_t(row) >= p.second.size()) {
			row -= p.second.size();
		} else {
			return std::make_tuple(p.first.client, p.second.at(row).first, p.second.at(row).second);
		}
	}

	throw std::runtime_error("Row index is out of range");
}

void SearchResultModel::remove_entries_from(std::string name)
{
	int start = 0;
	for(auto it = search_results.cbegin(); it != search_results.cend();) {
		if(it->first.client == name) {
			int count = it->second.size();
			call_on_delegates(&SearchResultUiDelegate::remove_entries_start, start, count);
			it = search_results.erase(it);
			call_on_delegates(&SearchResultUiDelegate::remove_entries_end);
		} else {
			start += it->second.size();
			++it;
		}
	}
}

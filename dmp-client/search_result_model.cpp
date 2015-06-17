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
	int count = 0;
	for(auto&& pair : response.results) {
		count += pair.second.size();
	}
	
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
		for(auto&& results_of_client : results.second) {
			size += results_of_client.second.size();
		}
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
		for(auto && pair : p.second) {
			if(row > 0 && size_t(row) >= pair.second.size()) {
				row -= pair.second.size();
			} else {
				return get_nth(boost::fusion::joint_view<dmp_library::LibraryFolder::tracklist_t::value_type const, Client const>(pair.second.at(row), p.first), column);
			}
		}
	}

	throw std::out_of_range("Row index was out of range.");
}

std::string SearchResultModel::header_data(int section) const
{
	if(section < 0 || size_t(section) >= number_of_library_entry_members + 1) {
		throw std::out_of_range("Column index was out of range.");
	}
	
	return get_nth_name<boost::fusion::joint_view<dmp_library::LibraryFolder::tracklist_t::value_type, Client>>(section);
}

void SearchResultModel::clear()
{
	search_results.clear();
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
		for(auto&& pair : p.second) {
			if(row > 0 && size_t(row) >= pair.second.size()) {
				row -= pair.second.size();
			} else {
				return std::make_tuple(p.first.client, pair.first, pair.second.at(row));	
			}
		}
	}

	throw std::runtime_error("Row index is out of range");
}

#include "search_result_model.hpp"
#include "fusion_static_dispatch.hpp"
#include "fusion_outputter.hpp"
#include "debug_macros.hpp"

#include <string>
#include <iostream>
#include <numeric>

SearchResultModel::SearchResultModel()
{
}

void SearchResultModel::add_search_response(message::SearchResponse response)
{
	call_on_delegates<SearchResultUiDelegate>(&SearchResultUiDelegate::search_results_start, response);
	search_results.push_back(std::make_pair(Client(response.origin), response.results));
	call_on_delegates<SearchResultUiDelegate>(&SearchResultUiDelegate::search_results_end);
}

int SearchResultModel::row_count() const
{
	return std::accumulate(search_results.cbegin(), search_results.cend(), 0, [](int acc, SearchResultsElement const& rh){
		return acc + std::accumulate(rh.second.cbegin(), rh.second.cend(), 0, [](int acc, SearchResultsElement::second_type::value_type const& rh) {
			return acc + rh.second.size();
		});  
	});
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
	call_on_delegates<SearchResultUiDelegate>(&SearchResultUiDelegate::new_search_begin);
	clear();
	current_query = query;
	call_on_delegates<SearchResultUiDelegate>(&SearchResultUiDelegate::new_search_end);
	
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

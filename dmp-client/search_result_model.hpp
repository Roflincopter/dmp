#pragma once

#include "library.hpp"

#include "friendly_fusion.hpp"
#include "delegator.hpp"

#include <boost/any.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/view/joint_view/joint_view.hpp>

#include <boost/mpl/int.hpp>

#include <stddef.h>
#include <cstdint>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace dmp_library { struct LibraryEntry; }

namespace message { struct SearchResponse; }

struct SearchResultUiDelegate;

class SearchResultModel : public Delegator<SearchResultUiDelegate>
{
public:
	struct Client
	{
		std::string client;
		
		Client(std::string client)
		: client(client)
		{}
	};
	
private:
	static constexpr size_t number_of_library_entry_members = friendly_fusion::result_of::size<dmp_library::LibraryEntry>::type::value;
	
	typedef std::pair<Client, std::vector<std::pair<size_t, dmp_library::LibraryEntry>>> SearchResultsElement;
	std::vector<SearchResultsElement> search_results;
	std::string current_query;

public:
	SearchResultModel();
	
	typedef boost::fusion::joint_view<dmp_library::LibraryEntry, Client> ElementType;

	void add_search_response(message::SearchResponse);
	void clear();
	
	void set_current_query(std::string query);
	std::string get_current_query() const;
	
	boost::any get_cell(int row, int column) const;
	std::string header_data(int section) const;
	int row_count() const;
	int column_count() const;

	std::tuple<std::string, uint32_t, dmp_library::LibraryEntry> get_row_info(int row) const;
	void remove_entries_from(std::string name);
};

BOOST_FUSION_ADAPT_STRUCT
(
	SearchResultModel::Client,
	(std::string, client)
)


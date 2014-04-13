#pragma once

#include "dmp-library.hpp"
#include "message.hpp"
#include "friendly_fusion.hpp"

#include <boost/any.hpp>

class SearchResultModel
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
	
	typedef std::pair<Client, dmp_library::Library::tracklist_t> SearchResultsElement;
	std::vector<SearchResultsElement> search_results;
	std::string current_query;

public:
	SearchResultModel();
	
	typedef boost::fusion::joint_view<dmp_library::Library::tracklist_t::value_type, Client> ElementType;

	void add_search_response(message::SearchResponse);
	void clear();
	
	void set_current_query(std::string query);
	std::string get_current_query() const;
	
	boost::any get_cell(int row, int column) const;
	std::string header_data(int section) const;
	int row_count() const;
	int column_count() const;

	std::pair<std::string, dmp_library::LibraryEntry> get_row_info(int row) const;
};

BOOST_FUSION_ADAPT_STRUCT
(
	SearchResultModel::Client,
	(std::string, client)
)


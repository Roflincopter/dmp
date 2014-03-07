#pragma once

#include "dmp-library.hpp"
#include "message.hpp"

#include <QAbstractTableModel>

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
	static constexpr size_t number_of_library_entry_members = boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value;
	
	typedef std::pair<Client, dmp_library::Library::tracklist_t> SearchResultsElement;
	std::vector<SearchResultsElement> search_results;

public:
	SearchResultModel();

	virtual void add_search_response(message::SearchResponse);
	virtual void clear();

	std::string get_cell(int row, int column) const;
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


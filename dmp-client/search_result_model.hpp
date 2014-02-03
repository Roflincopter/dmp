#pragma once

#include "dmp-library.hpp"
#include "message.hpp"

#include <QAbstractTableModel>

class SearchResultModel
{
	std::vector<std::pair<std::string, dmp_library::Library::tracklist_t>> search_results;

public:
	SearchResultModel();

	virtual void add_search_response(message::SearchResponse);
	virtual void clear();

	std::string get_cell(int row, int column) const;
	std::string header_data(int section) const;
	int row_count() const;
	int column_count() const;

	std::pair<std::string, dmp_library::LibraryEntry> get_row_info(int row);
};


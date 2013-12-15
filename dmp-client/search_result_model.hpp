#pragma once

#include "dmp-library.hpp"
#include "message.hpp"

#include <QAbstractTableModel>

namespace std {
	std::string to_string(std::string x);
}

class SearchResultModel
{
	std::vector<std::pair<std::string, dmp_library::Library>> search_results;

public:
	SearchResultModel();

	virtual void add_search_response(message::SearchResponse);
	virtual void clear();

	std::string get_cell(int row, int column) const;
	std::string header_data(int section) const;
	int row_count() const;
	int column_count() const;
};


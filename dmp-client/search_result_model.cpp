#include "search_result_model.hpp"
#include "fusion_static_dispatch.hpp"
#include "message_outputter.hpp"

#include <string>
#include <iostream>

BOOST_FUSION_ADAPT_STRUCT
(
    dmp_library::LibraryEntry,
    (std::string, artist)
    (std::string, ascii_artist)
    (std::string, title)
    (std::string, ascii_title)
    (std::string, album)
    (std::string, ascii_album)
    (uint32_t, track)
    (uint32_t, id)
)

void SearchResultModel::add_search_response(message::SearchResponse response)
{
    search_results.push_back(std::make_pair(response.origin, response.results));
}

SearchResultModel::SearchResultModel()
{
    typedef QVector<int> QIntVector;
    qRegisterMetaType<QIntVector>("QIntVector");
}

int SearchResultModel::row_count() const
{
    size_t rows = 0;
    for(auto e : search_results)
    {
        rows += e.second.size();
    }
    return rows;
}

int SearchResultModel::column_count() const
{
    return boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value;
}

std::string SearchResultModel::get_cell(int row, int column) const
{
    if(column >= boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value) {
        throw std::out_of_range("Column index was out of range.");
    }

    for(auto& p : search_results)
    {
        if(row > 0 && size_t(row) >= p.second.size()) {
            row -= p.second.size();
            continue;
        }
        return get_nth(p.second[row], column);
    }

    throw std::out_of_range("Row index was out of range.");
}

std::string SearchResultModel::header_data(int section) const
{
    if(section >= boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value) {
        throw std::out_of_range("Column index was out of range.");
    }

    return get_nth_name<dmp_library::LibraryEntry>(section);
}

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

void SearchResultModel::handle_search_response(std::string query, message::SearchResponse response)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount() - 1 + response.results.size());
    if(query != response.query) {
        return;
    }

    search_results.push_back(std::make_pair(response.origin, response.results));

    endInsertRows();
}

SearchResultModel::SearchResultModel()
{
    typedef QVector<int> QIntVector;
    qRegisterMetaType<QIntVector>("QIntVector");
}

int SearchResultModel::rowCount(const QModelIndex&) const
{
    size_t rows = 0;
    for(auto e : search_results)
    {
        rows += e.second.size();
    }
    return rows;
}

int SearchResultModel::columnCount(const QModelIndex&) const
{
    return boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value;
}

QVariant SearchResultModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if(!index.isValid() || index.column() >= boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value) {
        return QVariant();
    }

    int row = index.row();
    for(auto& p : search_results)
    {
        if(row > 0 && size_t(row) >= p.second.size()) {
            row -= p.second.size();
            continue;
        }
        return QVariant(QString::fromUtf8(get_nth(p.second[row], index.column()).c_str()));
    }
    return QVariant();
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Vertical) {
        return QVariant();
    }

    if(section < 0 || section >= boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value) {
        return QVariant();
    }
    return QVariant(QString::fromStdString(get_nth_name<dmp_library::LibraryEntry>(section)));
}

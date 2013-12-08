#pragma once

#include "dmp-library.hpp"
#include "message.hpp"

#include <QAbstractTableModel>

namespace std {
    std::string to_string(std::string x);
}

class SearchResultModel : public QAbstractTableModel
{
    Q_OBJECT

    std::vector<std::pair<std::string, dmp_library::Library>> search_results;

public:
    SearchResultModel();

    void handle_search_response(std::string query, message::SearchResponse);

    int rowCount(const QModelIndex & = QModelIndex()) const final;
    int columnCount(const QModelIndex &) const final;
    QVariant data(const QModelIndex &index, int role) const final;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const final;

};

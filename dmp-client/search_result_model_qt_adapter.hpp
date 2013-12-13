#pragma once

#include "search_result_model.hpp"

#include <QAbstractTableModel>

class SearchResultModelQtAdapter : public SearchResultModel, public QAbstractTableModel
{
public:
    SearchResultModelQtAdapter();

    virtual void add_search_response(message::SearchResponse search_response) final;

    int rowCount(const QModelIndex & = QModelIndex()) const final;
    int columnCount(const QModelIndex &) const final;
    QVariant data(const QModelIndex &index, int role) const final;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const final;
};

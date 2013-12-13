#include "search_result_model_qt_adapter.hpp"

SearchResultModelQtAdapter::SearchResultModelQtAdapter()
{
}

void SearchResultModelQtAdapter::add_search_response(message::SearchResponse search_response)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount() + search_response.results.size() - 1);
    SearchResultModel::add_search_response(search_response);
    endInsertRows();
}

int SearchResultModelQtAdapter::rowCount(const QModelIndex&) const
{
    return row_count();
}

int SearchResultModelQtAdapter::columnCount(const QModelIndex&) const
{
    return column_count();
}

QVariant SearchResultModelQtAdapter::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if(!index.isValid()) {
        return QVariant();
    }
    try {
        return QVariant(QString::fromStdString(get_cell(index.row(), index.column())));
    } catch (std::out_of_range e) {
        return QVariant();
    }
}

QVariant SearchResultModelQtAdapter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Vertical) {
        return QVariant();
    }

    if(section < 0) {
        return QVariant();
    }

    try {
        return QVariant(QString::fromStdString(header_data(section)));
    } catch(std::out_of_range e) {
        return QVariant();
    }
}

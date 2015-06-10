#pragma once

#include "dmp_qt_meta_types.hpp"

#include <QSortFilterProxyModel>

#include <memory>

class QModelIndex;
class SearchResultModelQtAdapter;

class SearchResultSortProxyModel : public QSortFilterProxyModel
{
	std::shared_ptr<SearchResultModelQtAdapter> model;

public:
	SearchResultSortProxyModel(std::shared_ptr<SearchResultModelQtAdapter> model);

	bool lessThan(const QModelIndex& left, const QModelIndex& right) const override final;

};

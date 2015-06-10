#include "search_result_sort_proxy_model.hpp"

#include "search_result_model_qt_adapter.hpp"

#include "library-entry.hpp"

#include <QAbstractItemModel>
#include <QMetaType>
#include <QVariant>

#include <string>

SearchResultSortProxyModel::SearchResultSortProxyModel(std::shared_ptr<SearchResultModelQtAdapter> model)
: model(model)
{
	setSourceModel(model.get());
}

bool SearchResultSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	using Duration = dmp_library::LibraryEntry::Duration;
	static int string_id = qMetaTypeId<std::string>();
	static int duration_id = qMetaTypeId<Duration>();

	if (leftData.userType() == string_id) {
		return leftData.value<std::string>() < rightData.value<std::string>();
	} else if (leftData.userType() == duration_id) {
		return leftData.value<Duration>() < rightData.value<Duration>();
	} else {
		return QSortFilterProxyModel::lessThan(left, right);
	}
}

#include "dmp_client_gui_search_results.hpp"

#include <playlist.hpp>

#include "search_result_model_qt_adapter.hpp"
#include "search_result_sort_proxy_model.hpp"

#include "dmp_client_interface.hpp"
#include "search_result_model.hpp"

#include <QHeaderView>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

#include <tuple>

class QWidget;

DmpClientGuiSearchResults::DmpClientGuiSearchResults(QWidget *parent)
: QTableView(parent)
, client(nullptr)
, model(std::make_shared<SearchResultModelQtAdapter>())
, proxy_model(std::make_shared<SearchResultSortProxyModel>(model))
, delegate()
{
	setModel(proxy_model.get());
	setItemDelegate(&delegate);
	hideColumn(1);
	hideColumn(3);
	hideColumn(5);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void DmpClientGuiSearchResults::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
	model->set_model(client->get_search_result_model());
	client->get_search_result_model()->add_delegate<SearchResultUiDelegate>(shared_from_this());
	client->get_search_result_model()->add_delegate<SearchResultUiDelegate>(model);
}

void DmpClientGuiSearchResults::queueSelection()
{
	auto selection = selectionModel()->selectedRows();
	std::vector<PlaylistEntry> pl_entries;
	for(auto row_index : selection) {
		
		QModelIndex index = proxy_model->mapToSource(row_index);
		auto row_info = model->get_row_info(index.row());
		pl_entries.emplace_back(client->get_name(), std::get<0>(row_info), std::get<1>(row_info));
	}
	
	client->queue(current_active_radio, pl_entries);
}

void DmpClientGuiSearchResults::currentActiveRadio(std::string radio_name)
{
	current_active_radio = radio_name;
}

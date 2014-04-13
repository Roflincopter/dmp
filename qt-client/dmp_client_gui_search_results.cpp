#include "dmp_client_gui_search_results.hpp"

#include <QHeaderView>

DmpClientGuiSearchResults::DmpClientGuiSearchResults(QWidget *parent)
: QTableView(parent)
, client(nullptr)
, model(std::make_shared<SearchResultModelQtAdapter>())
, delegate()
{
	setModel(model.get());
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
	client->add_delegate(model);
}

void DmpClientGuiSearchResults::queueRequest(QModelIndex index)
{
	auto x = model->get_row_info(index.row());
	client->queue(current_active_radio, x.first, x.second);
}

void DmpClientGuiSearchResults::currentActiveRadio(std::string radio_name)
{
	current_active_radio = radio_name;
}

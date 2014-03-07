#include "dmp_client_gui_search_results.hpp"

#include <QHeaderView>

DmpClientGuiSearchResults::DmpClientGuiSearchResults(QWidget *parent)
: QTableView(parent)
, client(nullptr)
, model()
{
	setModel(&model);
	hideColumn(1);
	hideColumn(3);
	hideColumn(5);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void DmpClientGuiSearchResults::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
}

void DmpClientGuiSearchResults::search_results(message::SearchResponse search_response)
{
	if(search_response.query != current_query) {
		return;
	}
	
	model.add_search_response(search_response);
}

void DmpClientGuiSearchResults::new_search(std::string query)
{
	current_query = query;
	model.clear();
}

void DmpClientGuiSearchResults::queueRequest(QModelIndex index)
{
	auto x = model.get_row_info(index.row());
	client->queue(current_active_radio, x.first, x.second);
}

void DmpClientGuiSearchResults::currentActiveRadio(std::string radio_name)
{
	current_active_radio = radio_name;
}

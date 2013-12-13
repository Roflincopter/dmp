#include "dmp_client_gui_search_results.hpp"

DmpClientGuiSearchResults::DmpClientGuiSearchResults(QWidget *parent)
: QTableView(parent)
, client(nullptr)
, model()
{
    setModel(&model);
}

void DmpClientGuiSearchResults::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
    client = new_client;
}

void DmpClientGuiSearchResults::search_results(message::SearchResponse search_response)
{
    model.add_search_response(search_response);
}

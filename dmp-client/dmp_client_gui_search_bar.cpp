#include "dmp_client_gui_search_bar.hpp"

DmpClientGuiSearchBar::DmpClientGuiSearchBar(QWidget *parent)
: QLineEdit(parent)
, client(nullptr)
, model()
{}

void DmpClientGuiSearchBar::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
}

void DmpClientGuiSearchBar::query_parse_error(dmp_library::ParseError e)
{
	model.set_data(e.expected, e.pivot);
	model.set_error_state(this);
}

void DmpClientGuiSearchBar::searchBarReturned()
{
	model.reset_error_state(this);
	client->search(text().toStdString());
}

void DmpClientGuiSearchBar::cursorChanged(int old_pos, int new_pos)
{
	model.model_check_state(new_pos, this);
}

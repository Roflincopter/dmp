#include "dmp_client_gui_search_bar.hpp"

DmpClientGuiSearchBar::DmpClientGuiSearchBar(QWidget *parent)
: QLineEdit(parent)
, client(nullptr)
{}

void DmpClientGuiSearchBar::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
    client = new_client;
}

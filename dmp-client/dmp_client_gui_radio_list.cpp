#include "dmp_client_gui_radio_list.hpp"

DmpClientGuiRadioList::DmpClientGuiRadioList(QWidget *parent)
: QListView(parent)
, client(nullptr)
{
}

void DmpClientGuiRadioList::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
    client = new_client;
}

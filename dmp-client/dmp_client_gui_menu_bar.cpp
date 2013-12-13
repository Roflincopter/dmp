#include "dmp_client_gui_menu_bar.hpp"

#include <QFileDialog>

DmpClientGuiMenuBar::DmpClientGuiMenuBar(QWidget *parent)
: QMenuBar(parent)
, client(nullptr)
{
}

void DmpClientGuiMenuBar::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
    client = new_client;
}

void DmpClientGuiMenuBar::indexFolder()
{
    std::string folder = QFileDialog::getExistingDirectory(this, tr("Select music folder to index")).toStdString();
    if(folder.empty()) {
       return;
    }

    client->index(folder);
}

#include "dmp_client_gui_menu_bar.hpp"

#include "library_dialog.hpp"

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

void DmpClientGuiMenuBar::editLibrary()
{
	LibraryDialog dialog(this);
	int result = dialog.exec();

	if(result == QDialog::Accepted) {
		config::clear_library();
		for(auto&& elem : dialog.get_library()) {
			config::add_library(elem.second);
		}
	}

	client->init_library();
}

#include "dmp_client_login_dialog.hpp"

#include "debug_macros.hpp"

DmpClientLoginDialog::DmpClientLoginDialog(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
}

std::string DmpClientLoginDialog::get_username()
{
	DEBUG_COUT << ui.Username->text().toStdString() << std::endl;
	return ui.Username->text().toStdString();
}

std::string DmpClientLoginDialog::get_password()
{
	DEBUG_COUT << ui.Password->text().toStdString() << std::endl;
	return ui.Password->text().toStdString();
}

#include "dmp_client_login_dialog.hpp"

#include "debug_macros.hpp"

#include <QPushButton>

DmpClientLoginDialog::DmpClientLoginDialog(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	auto button = ui.buttonBox->addButton("Register", QDialogButtonBox::ButtonRole::ActionRole);
	connect(button, &QAbstractButton::clicked, [this](){done(Register);});
}

std::string DmpClientLoginDialog::get_username()
{
	return ui.Username->text().toStdString();
}

std::string DmpClientLoginDialog::get_password()
{
	return ui.Password->text().toStdString();
}

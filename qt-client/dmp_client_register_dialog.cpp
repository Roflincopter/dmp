#include "dmp_client_register_dialog.hpp"

#include <QPushButton>

DmpClientRegisterDialog::DmpClientRegisterDialog(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	checkPassword();
}

std::string DmpClientRegisterDialog::get_username()
{
	return ui.username->text().toStdString();
}

std::string DmpClientRegisterDialog::get_password()
{
	return ui.password->text().toStdString();
}

void DmpClientRegisterDialog::checkPassword()
{
	auto button = ui.buttonBox->button(QDialogButtonBox::Ok);
	
	bool enabled = ui.password->text().size() >= 8 
		&& ui.password->text() == ui.confirmPassword->text();
	
	button->setEnabled(enabled);
}

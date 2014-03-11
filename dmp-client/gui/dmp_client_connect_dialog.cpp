#include "dmp_client_connect_dialog.hpp"

#include <cassert>

#include <iostream>

#include <QPushButton>

DmpClientConnectDialog::DmpClientConnectDialog(QWidget *parent)
: QDialog(parent)
, port_validator(this)
{
	ui.setupUi(this);
	updateOkButton();

	ui.PortEdit->setValidator(&port_validator);
}

void DmpClientConnectDialog::set_default_name(std::string name)
{
	ui.NameEdit->setText(QString::fromStdString(name));
}

std::string DmpClientConnectDialog::get_name()
{
	return ui.NameEdit->text().toStdString();
}

std::string DmpClientConnectDialog::get_host()
{
	return ui.HostEdit->text().toStdString();
}

uint16_t DmpClientConnectDialog::get_port()
{
	assert(sizeof(uint16_t) == sizeof(ui.PortEdit->text().toUShort()));
	return ui.PortEdit->text().toUShort();
}

void DmpClientConnectDialog::updateOkButton()
{
	// We purposedly not validate host & port beyond checking for not-emptiness
	// host is too hard to validate (everything can be a hostname)
	// port is validated by the QValidator on the field
	// name is just something that must not be empty.

	bool const name_valid = !get_name().empty();
	bool const host_valid = !get_host().empty();
	bool const port_valid = !ui.PortEdit->text().isEmpty();
	bool const ok_enabled = name_valid && host_valid && port_valid;

	ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok_enabled);
}

void DmpClientConnectDialog::inputChanged()
{
	updateOkButton();
}

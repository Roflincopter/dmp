#include "dmp_client_connect_dialog.hpp"

#include <cassert>

#include <iostream>

#include <QPushButton>

DmpClientConnectDialog::DmpClientConnectDialog(QWidget *parent)
: QDialog(parent)
, port_validator(this)
, servers(config::get_servers())
{
	ui.setupUi(this);
	updateOkButton();

	ui.PortEdit->setValidator(&port_validator);
	
	for(auto&& server : servers) {
		ui.Servers->addItem(QString::fromStdString(server.second.get_value<std::string>()));
	}
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

	bool const host_valid = !get_host().empty();
	bool const port_valid = !ui.PortEdit->text().isEmpty();
	bool const ok_enabled = host_valid && port_valid;

	ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok_enabled);
}

void DmpClientConnectDialog::inputChanged()
{
	updateOkButton();
}

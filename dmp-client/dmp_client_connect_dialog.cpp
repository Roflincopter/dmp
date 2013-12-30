#include "dmp_client_connect_dialog.hpp"

#include <cassert>

DmpClientConnectDialog::DmpClientConnectDialog(QWidget *parent)
: QDialog(parent)
, port_validator(this)
{
	ui.setupUi(this);

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

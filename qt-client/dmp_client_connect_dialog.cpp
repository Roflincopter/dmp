#include "dmp_client_connect_dialog.hpp"

#include "fusion_outputter.hpp"
#include "debug_macros.hpp"

#include <cassert>

#include <iostream>

#include <QPushButton>

DmpClientConnectDialog::DmpClientConnectDialog(QWidget *parent)
: QDialog(parent)
, port_validator(this)
, servers()
{
	ui.setupUi(this);
	updateOkButton();

	ui.PortEdit->setValidator(&port_validator);
	
	for(auto&& server : config::get_servers()) {
		std::string name = server.second.get<std::string>("name");
	
		servers[name] = ConnectionInfo {
			server.second.get<std::string>("hostname"),
			server.second.get<std::string>("port")
		};
		
		ui.Servers->addItem(QString::fromStdString(name));
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

void DmpClientConnectDialog::nameChanged(QString str)
{
	QListWidgetItem* item = ui.Servers->item(ui.Servers->currentIndex().row());
	servers[str.toStdString()] = servers[item->text().toStdString()];
	servers.erase(item->text().toStdString());
	item->setText(str);
}

void DmpClientConnectDialog::hostChanged(QString str)
{
	QListWidgetItem* item = ui.Servers->item(ui.Servers->currentIndex().row());
	servers[item->text().toStdString()].host_name = str.toStdString();
}

void DmpClientConnectDialog::portChanged(QString str)
{
	QListWidgetItem* item = ui.Servers->item(ui.Servers->currentIndex().row());
	servers[item->text().toStdString()].port = str.toStdString();
}

void DmpClientConnectDialog::inputChanged()
{
	updateOkButton();
}

void DmpClientConnectDialog::addPressed()
{
	std::string new_label = "new server";
	int no = 1;
	while(servers.find(new_label) != servers.end()) {
		std::stringstream ss;
		ss << "new server " << no;
		new_label = ss.str();
		++no;
	}
	
	ui.Servers->addItem(QString::fromStdString(new_label));
	servers[new_label] = {};
	auto index = ui.Servers->rootIndex().child(ui.Servers->count() - 1, 0);
	ui.Servers->setCurrentIndex(index);
}

void DmpClientConnectDialog::deletePressed()
{
	auto items = ui.Servers->selectedItems();
	if(items.length() == 1) {
		auto item = items[0];
		
		std::string text = item->text().toStdString();
		servers.erase(text);
		
		delete item;
		selectionChanged();
	}
}

void DmpClientConnectDialog::selectionChanged()
{
	auto items = ui.Servers->selectedItems();
	if(items.length() == 1) {
		auto item = items[0];
		std::string name = item->text().toStdString();
		
		ui.NameEdit->setText(QString::fromStdString(name));
		ui.HostEdit->setText(QString::fromStdString(servers[name].host_name));
		ui.PortEdit->setText(QString::fromStdString(servers[name].port));
	}
	
}

void DmpClientConnectDialog::afterAccept()
{
	config::clear_servers();
	for(auto&& server : servers) {
		config::add_server(server.first, server.second.host_name, server.second.port);
	}
}

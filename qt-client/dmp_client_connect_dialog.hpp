#pragma once

#include "ui_dmp_client_connect_dialog.hpp"

#include "dmp_config.hpp"

#include <memory>

struct ConnectionInfo {
	std::string host_name;
	std::string port;
	
	friend std::ostream& operator<<(std::ostream& os, ConnectionInfo c) {
		return os << "{host_name: " << "port: " << c.host_name << ", " << c.port << "}";
	}
};

class DmpClientConnectDialog : public QDialog
{
	Q_OBJECT

	QIntValidator port_validator{1, std::numeric_limits<uint16_t>::max(), this};

public:
	explicit DmpClientConnectDialog(QWidget *parent = 0);

	std::string get_host();
	uint16_t get_port();
	std::map<std::string, ConnectionInfo> servers;

private:
	Ui::DmpClientConnectDialog ui;

	//! Checks if current input is acceptable, enables/disables the ok-button
	void updateOkButton();

	void setEnabled(bool enable);

private slots:
	void inputChanged();
	void nameChanged(QString str);
	void hostChanged(QString str);
	void portChanged(QString str);
	void addPressed();
	void deletePressed();
	void selectionChanged();
	
	void saveToConfig();
	void afterAccept();
};

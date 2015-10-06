#pragma once

#include "ui_dmp_client_connect_dialog.hpp"

#include <QtGui/qvalidator.h>
#include <QDialog>
#include <qobjectdefs.h>
#include <QString>

#include <stdint.h>
#include <limits>
#include <map>
#include <ostream>
#include <string>

struct ConnectionInfo {
	std::string host_name;
	std::string port;
	bool secure;
	
	friend std::ostream& operator<<(std::ostream& os, ConnectionInfo c) {
		return os << "{host_name: " << "port: " << c.host_name << ", " << c.port << " using secured connection: " << (c.secure ? "yes" : "no") << "}";
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
	bool get_secure();
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
	void secureChanged(bool checked);
	void addPressed();
	void deletePressed();
	void selectionChanged();
	
	void saveToConfig();
	void afterAccept();
};

#pragma once

#include "ui_dmp_client_connect_dialog.hpp"

#include <memory>

class DmpClientConnectDialog : public QDialog
{
	Q_OBJECT

	QIntValidator port_validator{1, std::numeric_limits<uint16_t>::max(), this};

public:
	explicit DmpClientConnectDialog(QWidget *parent = 0);

	void set_default_name(std::string name);

	std::string get_name();
	std::string get_host();
	uint16_t get_port();

private:
	Ui::DmpClientConnectDialog ui;
};

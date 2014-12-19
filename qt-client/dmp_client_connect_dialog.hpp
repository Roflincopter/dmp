#pragma once

#include "ui_dmp_client_connect_dialog.hpp"

#include "dmp_config.hpp"

#include <memory>

class DmpClientConnectDialog : public QDialog
{
	Q_OBJECT

	QIntValidator port_validator{1, std::numeric_limits<uint16_t>::max(), this};

public:
	explicit DmpClientConnectDialog(QWidget *parent = 0);

	std::string get_host();
	uint16_t get_port();
	config::array servers;

private:
	Ui::DmpClientConnectDialog ui;

	//! Checks if current input is acceptable, enables/disables the ok-button
	void updateOkButton();

private slots:
	void inputChanged();
};

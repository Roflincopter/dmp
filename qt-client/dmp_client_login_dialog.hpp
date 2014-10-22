#pragma once

#include "ui_dmp_client_login_dialog.hpp"

class DmpClientLoginDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit DmpClientLoginDialog(QWidget *parent = 0);
	
	enum {
		Register = 2
	};
	
	std::string get_username();
	std::string get_password();
	
private:
	Ui::DmpClientLoginDialog ui;
};

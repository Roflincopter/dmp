#pragma once

#include "ui_dmp_client_register_dialog.hpp"

#include <QDialog>

#include <string>

class QWidget;

class DmpClientRegisterDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit DmpClientRegisterDialog(QWidget *parent = 0);
	
	std::string get_username();
	std::string get_password();
	
private slots:
	void checkPassword();
	
private:
	Ui::DmpClientRegisterDialog ui;
};

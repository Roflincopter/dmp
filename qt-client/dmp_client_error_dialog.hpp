#pragma once

#include "ui_dmp_client_error_dialog.hpp"

#include <memory>

class DmpClientErrorDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DmpClientErrorDialog(std::string error, QWidget *parent = 0);

	std::string get_error();

private:
	Ui::DmpClientErrorDialog ui;
};

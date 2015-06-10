#pragma once

#include "ui_dmp_client_error_dialog.hpp"

#include <QDialog>
#include <qobjectdefs.h>

#include <string>

class QWidget;

class DmpClientErrorDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DmpClientErrorDialog(std::string error, QWidget *parent = 0);

	std::string get_error();

private:
	Ui::DmpClientErrorDialog ui;
};

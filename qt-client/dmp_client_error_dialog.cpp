#include "dmp_client_error_dialog.hpp"

#include <QPlainTextEdit>
#include <QString>

class QWidget;

DmpClientErrorDialog::DmpClientErrorDialog(std::string error, QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	ui.errorEdit->setPlainText(QString::fromStdString(error));
}

std::string DmpClientErrorDialog::get_error()
{
	return ui.errorEdit->toPlainText().toStdString();
}

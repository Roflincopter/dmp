#pragma once

#include <QDialog>

#include "dmp_config.hpp"

namespace Ui {
class LibraryDialog;
}

class LibraryDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LibraryDialog(QWidget *parent = 0);
	~LibraryDialog();

private slots:
	void addPressed();
	void deletePressed();

private:
	Ui::LibraryDialog *ui;
};

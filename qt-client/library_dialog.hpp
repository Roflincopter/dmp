#pragma once

#include "dmp_config.hpp"

#include <QDialog>

#include <vector>

namespace Ui {
class LibraryDialog;
}

class LibraryDialog : public QDialog
{
	Q_OBJECT

	std::map<std::string, config::LibraryInfo> library;

	void setEnabled(bool enable);
	void updateOkButton();
	std::string get_path();
	std::string get_name();

public:
	explicit LibraryDialog(QWidget *parent = 0);
	~LibraryDialog();

	std::map<std::string, config::LibraryInfo> get_library();

private slots:
	void addPressed();
	void deletePressed();
	void selectionChanged();

	void folderSelectButtonClicked();

	void inputChanged();
	void pathChanged(QString str);
	void nameChanged(QString str);
private:
	Ui::LibraryDialog* ui;
};

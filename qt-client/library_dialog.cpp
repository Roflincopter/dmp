#include "library_dialog.hpp"
#include "ui_library_dialog.hpp"

LibraryDialog::LibraryDialog(QWidget *parent)
: QDialog(parent)
, ui(new Ui::LibraryDialog)
{
	ui->setupUi(this);
}

LibraryDialog::~LibraryDialog()
{
	delete ui;
}

void LibraryDialog::addPressed()
{

}

void LibraryDialog::deletePressed()
{

}

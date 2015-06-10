#include "library_dialog.hpp"
#include "ui_library_dialog.hpp"

#include <QPushButton>
#include <QFileDialog>
#include <QAbstractItemModel>
#include <QDialogButtonBox>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>

class QWidget;

LibraryDialog::LibraryDialog(QWidget *parent)
: QDialog(parent)
, library()
, ui(new Ui::LibraryDialog)
{
	ui->setupUi(this);

	auto library_info = config::get_library_information();
	for(auto&& library_entry : library_info) {
		library[library_entry.name] = library_entry;

		ui->LibraryList->addItem(QString::fromStdString(library_entry.name));
	}

	if(ui->LibraryList->count() != 0) {
		auto const& index = ui->LibraryList->model()->index(0, 0);
		ui->LibraryList->selectionModel()->select(index, QItemSelectionModel::Select);
		emit selectionChanged();
	}
}

LibraryDialog::~LibraryDialog()
{
	delete ui;
}

std::map<std::string, config::LibraryInfo> LibraryDialog::get_library()
{
	return library;
}

void LibraryDialog::setEnabled(bool enable)
{
	ui->NameEdit->setEnabled(enable);
	ui->PathEdit->setEnabled(enable);
	ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(enable);
}

std::string LibraryDialog::get_name()
{
	return ui->NameEdit->text().toStdString();
}

std::string LibraryDialog::get_path()
{
	return ui->PathEdit->text().toStdString();
}

inline QListWidgetItem* get_selected_item(QListWidget* list) {
	auto selection = list->selectionModel()->selectedRows();
	if(selection.size() == 1) {
		return list->item(selection[0].row());
	} else {
		return nullptr;
	}
}

void LibraryDialog::nameChanged(QString str)
{
	if(auto item = get_selected_item(ui->LibraryList)) {
		library[str.toStdString()] = library[item->text().toStdString()];
		library[str.toStdString()].name = str.toStdString();
		library.erase(item->text().toStdString());
		item->setText(str);
	}
}

void LibraryDialog::pathChanged(QString str)
{
	if(auto item = get_selected_item(ui->LibraryList)) {
		library[item->text().toStdString()].path = str.toStdString();
	}
}

void LibraryDialog::inputChanged()
{
	updateOkButton();
}

void LibraryDialog::updateOkButton()
{
	// We purposedly not validate host & port beyond checking for not-emptiness
	// host is too hard to validate (everything can be a hostname)
	// port is validated by the QValidator on the field
	// name is just something that must not be empty.

	std::vector<bool> ok;
	for(auto&& elem : library) {
		ok.push_back(!elem.second.name.empty());
		ok.push_back(boost::filesystem::exists(elem.second.path) && boost::filesystem::is_directory(elem.second.path));
	}

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(std::all_of(ok.begin(), ok.end(), [](bool elem){return elem;}));
}

void LibraryDialog::addPressed()
{
	std::string new_label = "new folder";
	int no = 1;
	while(library.find(new_label) != library.end()) {
		std::stringstream ss;
		ss << "new server " << no;
		new_label = ss.str();
		++no;
	}

	ui->LibraryList->addItem(QString::fromStdString(new_label));
	library[new_label] = {new_label, "", config::get_unique_cache_name().string()};
	auto index = ui->LibraryList->rootIndex().child(ui->LibraryList->count() - 1, 0);
	ui->LibraryList->selectionModel()->select(index, QItemSelectionModel::SelectionFlag::Select);
	selectionChanged();
	updateOkButton();
}

void LibraryDialog::deletePressed()
{
	auto items = ui->LibraryList->selectedItems();
	if(items.length() == 1) {
		auto item = items[0];

		std::string text = item->text().toStdString();
		std::string cache_file = library[text].cache_file;

		boost::filesystem::path cache_path = config::get_library_folder_name() / cache_file;
		if(boost::filesystem::exists(cache_path)) {
			boost::filesystem::remove(cache_path);
		}

		library.erase(text);

		delete item;
		selectionChanged();
	}
	updateOkButton();
}

void LibraryDialog::selectionChanged() {
	auto items = ui->LibraryList->selectedItems();
	if(items.length() == 1) {
		auto item = items[0];
		std::string name = item->text().toStdString();

		ui->NameEdit->setText(QString::fromStdString(name));
		ui->PathEdit->setText(QString::fromStdString(library[name].path));
		setEnabled(true);
	} else {
		setEnabled(false);
	}
	updateOkButton();
}

void LibraryDialog::folderSelectButtonClicked()
{
	QString path = QFileDialog::getExistingDirectory(
		this,
		"Select a Music folder",
		ui->PathEdit->text()
	);
	
	ui->PathEdit->setText(path);
	pathChanged(path);
	updateOkButton();
}

#include "dmp_qt_meta_types.hpp"

#include "gui_item_delegate.hpp"

GuiItemDelegate::GuiItemDelegate(QObject *parent) :
QStyledItemDelegate(parent)
{
}

QString GuiItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
	QString ret;
	static int string_id = qMetaTypeId<std::string>();
	static int duration_id = qMetaTypeId<dmp_library::LibraryEntry::Duration>();
	
	if(string_id == value.userType()) {
		ret = QString::fromStdString(value.value<std::string>());
	} else if(duration_id == value.userType()) {
		ret = QString::fromStdString(value.value<dmp_library::LibraryEntry::Duration>().to_string());
	} else {
		ret = QStyledItemDelegate::displayText(value, locale);
	}
	return ret;
}

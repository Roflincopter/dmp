#include "search_bar_model_qt_adapter.hpp"

#include <iostream>

SearchBarModelQtAdapter::SearchBarModelQtAdapter()
: expected(get_expected())
, pivot(get_pivot())
{

}

void SearchBarModelQtAdapter::set_line_edit_text_format(QLineEdit *search_bar, const std::vector<QTextLayout::FormatRange> formats)
{
	if(!search_bar) {
		return;
	}

	QList<QInputMethodEvent::Attribute> attributes;
	for(const QTextLayout::FormatRange& fr : formats)
	{
		QInputMethodEvent::AttributeType type = QInputMethodEvent::TextFormat;
		int start = fr.start - search_bar->cursorPosition();
		int length = fr.length;
		QVariant value = fr.format;
		attributes.append(QInputMethodEvent::Attribute(type, start, length, value));
	}
	QInputMethodEvent event(QString(), attributes);
	QCoreApplication::sendEvent(search_bar, &event);
}

void SearchBarModelQtAdapter::reset_error_state(QLineEdit* search_bar)
{
	set_line_edit_text_format(search_bar, {});
}

void SearchBarModelQtAdapter::set_error_state(QLineEdit *search_bar)
{
	std::vector<QTextLayout::FormatRange> formats;

	//Correct portion
	QTextCharFormat correct;
	correct.setForeground(Qt::green);

	QTextLayout::FormatRange fr_correct;
	fr_correct.start = 0;
	fr_correct.length = pivot;
	fr_correct.format = correct;

	//Incorrect portion
	QTextCharFormat incorrect;
	incorrect.setForeground(Qt::red);

	QTextLayout::FormatRange fr_incorrect;
	fr_incorrect.start = pivot;
	fr_incorrect.length = search_bar->text().length() - pivot;
	fr_incorrect.format = incorrect;

	//Set the text formats.
	formats.push_back(fr_correct);
	formats.push_back(fr_incorrect);

	set_line_edit_text_format(search_bar, formats);
}

void SearchBarModelQtAdapter::set_data(std::string new_expected, size_t new_pivot)
{
	SearchBarModel::set_data(new_expected, new_pivot);
}

void SearchBarModelQtAdapter::model_check_state(int cursor_pos, QLineEdit* search_bar)
{
	if(cursor_pos < 0 || size_t(cursor_pos) < pivot || cursor_pos == 0) {
		reset_error_state(search_bar);
	}
}

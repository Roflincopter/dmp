#include "search_bar_model_qt_adapter.hpp"

#include <iostream>

SearchBarModelQtAdapter::SearchBarModelQtAdapter()
{}

std::vector<QTextLayout::FormatRange> SearchBarModelQtAdapter::get_error_formats()
{
	std::vector<QTextLayout::FormatRange> formats;

	//Correct portion
	QTextCharFormat correct;
	correct.setForeground(Qt::green);

	QTextLayout::FormatRange fr_correct;
	fr_correct.start = 0;
	fr_correct.length = model->get_pivot();
	fr_correct.format = correct;

	//Incorrect portion
	QTextCharFormat incorrect;
	incorrect.setForeground(Qt::red);

	QTextLayout::FormatRange fr_incorrect;
	fr_incorrect.start = model->get_pivot();
	fr_incorrect.length = model->get_query_length() - model->get_pivot();
	fr_incorrect.format = incorrect;

	//Set the text formats.
	formats.push_back(fr_correct);
	formats.push_back(fr_incorrect);

	return formats;
}

bool SearchBarModelQtAdapter::should_reset_error_state(int cursor_pos)
{
	return cursor_pos < 0 || size_t(cursor_pos) < model->get_pivot() || cursor_pos == 0;
}

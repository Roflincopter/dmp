#pragma once

#include "qt_adapter.hpp"

#include <QTextLayout>

#include <vector>

class SearchBarModel;

struct SearchBarModelQtAdapter : public QtAdapter<SearchBarModel>
{
	SearchBarModelQtAdapter();

	std::vector<QTextLayout::FormatRange> get_error_formats();
	bool should_reset_error_state(int cursor_pos);
};

#pragma once

#include "qt_adapter.hpp"
#include "search_bar_model.hpp"
#include "dmp_client_ui_delegate.hpp"

#include <QLineEdit>
#include <QTextLayout>
#include <QCoreApplication>

#include <memory>

struct SearchBarModelQtAdapter : public QtAdapter<SearchBarModel>
{
	SearchBarModelQtAdapter();

	std::vector<QTextLayout::FormatRange> get_error_formats();
	bool should_reset_error_state(int cursor_pos);
};

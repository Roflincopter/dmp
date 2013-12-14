#pragma once

#include "search_bar_model.hpp"

#include <QLineEdit>
#include <QTextLayout>
#include <QCoreApplication>

class SearchBarModelQtAdapter : public SearchBarModel
{
    std::string& expected;
    size_t& pivot;

public:
    SearchBarModelQtAdapter();

    void set_line_edit_text_format(QLineEdit* search_bar, const std::vector<QTextLayout::FormatRange> formats);
    void reset_error_state(QLineEdit* search_bar);
    void set_error_state(QLineEdit* search_bar);

    virtual void set_data(std::string new_expected, size_t new_pivot);

    void model_check_state(int cursor_pos, QLineEdit* search_bar);
};

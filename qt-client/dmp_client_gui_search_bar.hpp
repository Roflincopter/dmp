#pragma once

#include "dmp_client_ui_controller_interface.hpp"
#include "search_bar_ui_delegate.hpp"

#include <QLineEdit>
#include <QTextLayout>

#include <memory>
#include <vector>

class QWidget;
struct DmpClientInterface;
struct SearchBarModelQtAdapter;

class DmpClientGuiSearchBar
: public QLineEdit
, public DmpClientUiControllerInterface
, public SearchBarUiDelegate
, public std::enable_shared_from_this<SearchBarUiDelegate>
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;
	std::shared_ptr<SearchBarModelQtAdapter> model;

	void set_line_edit_text_format(const std::vector<QTextLayout::FormatRange> formats);
	void reset_error_state();
	
public:
	explicit DmpClientGuiSearchBar(QWidget *parent = 0);

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) override final;

	virtual void query_parse_error() override final;
signals:

public slots:
	void searchBarReturned();
	void cursorChanged(int old_pos, int new_pos);
};

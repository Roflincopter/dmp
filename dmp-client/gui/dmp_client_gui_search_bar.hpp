#pragma once

#include "dmp_client_ui_controller_interface.hpp"
#include "search_bar_model_qt_adapter.hpp"

#include <QLineEdit>

class DmpClientGuiSearchBar : public QLineEdit, public DmpClientUiControllerInterface, public DmpClientUiDelegate
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;
	SearchBarModelQtAdapter model;

public:
	explicit DmpClientGuiSearchBar(QWidget *parent = 0);

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) final;

	virtual void query_parse_error(dmp_library::ParseError e) final;
signals:

public slots:
	void searchBarReturned();
	void cursorChanged(int old_pos, int new_pos);
};

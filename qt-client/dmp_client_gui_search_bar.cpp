#include "dmp_client_gui_search_bar.hpp"

void DmpClientGuiSearchBar::set_line_edit_text_format(const std::vector<QTextLayout::FormatRange> formats)
{
	QList<QInputMethodEvent::Attribute> attributes;
	for(const QTextLayout::FormatRange& fr : formats)
	{
		QInputMethodEvent::AttributeType type = QInputMethodEvent::TextFormat;
		int start = fr.start - cursorPosition();
		int length = fr.length;
		QVariant value = fr.format;
		attributes.append(QInputMethodEvent::Attribute(type, start, length, value));
	}
	
	QInputMethodEvent event(QString(), attributes);
	QCoreApplication::sendEvent(this, &event);
}

void DmpClientGuiSearchBar::reset_error_state()
{
	set_line_edit_text_format({});
}

DmpClientGuiSearchBar::DmpClientGuiSearchBar(QWidget *parent)
: QLineEdit(parent)
, client(nullptr)
, model(std::make_shared<SearchBarModelQtAdapter>())
{}

void DmpClientGuiSearchBar::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
	model->set_model(client->get_search_bar_model());
	client->get_search_bar_model()->add_delegate<SearchBarUiDelegate>(shared_from_this());
}

void DmpClientGuiSearchBar::query_parse_error()
{
	reset_error_state();
	set_line_edit_text_format(model->get_error_formats());
}

void DmpClientGuiSearchBar::searchBarReturned()
{
	reset_error_state();
	client->search(text().toStdString());
}

void DmpClientGuiSearchBar::cursorChanged(int __attribute__((unused)) old_pos, int new_pos)
{
	if(model->should_reset_error_state(new_pos)) {
		reset_error_state();
	}
}

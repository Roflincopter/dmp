#pragma once

#include "ui_dmp_client_connect_dialog.hpp"

#include <memory>

struct PortValidator : public QIntValidator {
	PortValidator(QObject * parent)
	: QIntValidator(1, std::numeric_limits<uint16_t>::max(), parent)
	{}

	QValidator::State validate(QString &input, int &pos) const final
	{
		QIntValidator::State result(QIntValidator::validate(input, pos));
		if (result == QValidator::Intermediate) {
			result = QValidator::Invalid;
		}

		if(input.isEmpty()) {
			result = QIntValidator::State::Acceptable;
		}

		return result;
	}
};

class DmpClientConnectDialog : public QDialog
{
	Q_OBJECT

	PortValidator port_validator;

public:
	explicit DmpClientConnectDialog(QWidget *parent = 0);

	void set_default_name(std::string name);

	std::string get_name();
	std::string get_host();
	uint16_t get_port();

private:
	Ui::DmpClientConnectDialog ui;
};

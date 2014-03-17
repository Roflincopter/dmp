#pragma once

#include <QStyledItemDelegate>

class GuiItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit GuiItemDelegate(QObject *parent = 0);
	virtual ~GuiItemDelegate() = default;
	
	virtual QString displayText(const QVariant &value, const QLocale &locale) const override final;
	
signals:
	
public slots:
	
};

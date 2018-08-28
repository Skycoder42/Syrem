#ifndef KDESNOOZEDIALOG_H
#define KDESNOOZEDIALOG_H

#include <QInputDialog>
#include <remindmelib.h>
#include <eventexpressionparser.h>
#include <syncedsettings.h>

class KdeSnoozeDialog : public QInputDialog
{
	Q_OBJECT

public:
	explicit KdeSnoozeDialog(SyncedSettings *settings,
							 EventExpressionParser *parser,
							 const QString &description,
							 QWidget *parent = nullptr);

	void accept() override;

signals:
	void timeSelected(const QDateTime &time);

private:
	EventExpressionParser *_parser;
};

#endif // KDESNOOZEDIALOG_H

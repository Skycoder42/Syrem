#ifndef KDESNOOZEDIALOG_H
#define KDESNOOZEDIALOG_H

#include <QInputDialog>
#include <remindmelib.h>
#include <dateparser.h>
#include <syncedsettings.h>

class KdeSnoozeDialog : public QInputDialog
{
	Q_OBJECT

public:
	explicit KdeSnoozeDialog(SyncedSettings *settings,
							 DateParser *parser,
							 const QString &description,
							 QWidget *parent = nullptr);

	void accept() override;

signals:
	void timeSelected(const QDateTime &time);

private:
	DateParser *_parser;
};

#endif // KDESNOOZEDIALOG_H

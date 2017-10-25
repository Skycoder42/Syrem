#ifndef SNOOZEDIALOG_H
#define SNOOZEDIALOG_H

#include <QDateTime>
#include <QInputDialog>
#include <QSettings>
#include <dateparser.h> //direct use ok here, as it's part of a daemon service...

class KdeSnoozeDialog : public QInputDialog
{
	Q_OBJECT

public:
	explicit KdeSnoozeDialog(const QString &text, QWidget *parent = nullptr);

	QDateTime snoozeTime() const;

	void accept() override;

private:
	QSettings *_settings;
	DateParser *_parser;
	QDateTime _nextTime;
};

#endif // SNOOZEDIALOG_H

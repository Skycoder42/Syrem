#ifndef WIDGETSSNOOZEDIALOG_H
#define WIDGETSSNOOZEDIALOG_H

#include <QDialog>
#include <QToolBox>
#include <reminder.h>
#include <dateparser.h> //direct use ok here, as it's part of a daemon service...
#include <syncedsettings.h>

class WidgetsSnoozeDialog : public QDialog
{
	Q_OBJECT

public:
	explicit WidgetsSnoozeDialog(SyncedSettings *settings, QWidget *parent = nullptr);

	void addReminders(const QList<Reminder> &reminders);

public slots:
	void reject() override;

signals:
	void reacted(Reminder reminder, bool completed, const QDateTime &snoozeTime = {});
	void completed(const QList<Reminder> &remainingReminders);

private slots:
	void performComplete();
	void performSnooze();

private:
	SyncedSettings *_settings;
	QToolBox *_toolBox;

	QHash<QWidget*, Reminder> _reminders;
	DateParser *_parser;

	void setupUi();
	void resizeUi();
	void addReminder(const Reminder reminder);
};

#endif // WIDGETSSNOOZEDIALOG_H

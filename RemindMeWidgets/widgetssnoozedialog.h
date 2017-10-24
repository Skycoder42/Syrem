#ifndef SNOOZEDIALOG_H
#define SNOOZEDIALOG_H

#include <QDialog>
#include <QToolBox>
#include <reminder.h>
#include <dateparser.h> //direct use ok here, as it's part of a daemon service...

class WidgetsSnoozeDialog : public QDialog
{
	Q_OBJECT

public:
	enum Action {
		CompleteAction,
		DefaultSnoozeAction,
		SnoozeAction
	};
	Q_ENUM(Action)

	explicit WidgetsSnoozeDialog(bool showDefaults, QWidget *parent = nullptr);

	void addReminders(const QList<Reminder> &reminders);

public slots:
	void reject() override;

signals:
	void reacted(Reminder reminder, Action action, const QDateTime &snoozeTime = {});
	void aborted(const QList<Reminder> &reminders);

private slots:
	void performComplete();
	void performDefaultSnooze();
	void performSnooze();

private:
	const bool _showDefaults;
	QToolBox *_toolBox;

	QHash<QWidget*, Reminder> _reminders;
	DateParser *_parser;

	void setupUi();
	void resizeUi();
	void addReminder(const Reminder reminder);

	QDateTime tryParse(const QString &text);
};

#endif // SNOOZEDIALOG_H

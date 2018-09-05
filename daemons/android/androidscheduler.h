#ifndef ANDROIDSCHEDULER_H
#define ANDROIDSCHEDULER_H

#include <QObject>
#include <QAndroidJniObject>
#include <reminder.h>
class SyncedSettings;

class AndroidScheduler : public QObject
{
	Q_OBJECT

public:
	explicit AndroidScheduler(QObject *parent = nullptr);

	void setupAutoCheck(int interval);
	void disableAutoCheck();

	bool scheduleReminder(const Reminder &reminder);
	void cancleReminder(const QUuid &id);

private:
	QAndroidJniObject _jScheduler;
};

#endif // ANDROIDSCHEDULER_H

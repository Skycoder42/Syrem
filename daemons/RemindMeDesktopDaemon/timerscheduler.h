#ifndef WIDGETSSCHEDULER_H
#define WIDGETSSCHEDULER_H

#include <QObject>
#include <QTimer>
#include <reminder.h>

class TimerScheduler : public QObject
{
	Q_OBJECT

public:
	explicit TimerScheduler(QObject *parent = nullptr);

public slots:
	void initialize(const QList<Reminder> &allReminders);
	void scheduleReminder(const Reminder &reminder);
	void cancleReminder(const QUuid &id);

signals:
	void scheduleTriggered(const QUuid &id);

protected:
	void timerEvent(QTimerEvent *event) override;

private:
	struct SchedInfo {
		quint32 version;
		QDateTime date;
		int timerId;
	};
	QHash<QUuid, SchedInfo> _schedules;

	int _loopTimerId;

	void reschedule();
	int trySchedule(const QDateTime &target, const QUuid &id);
};

#endif // WIDGETSSCHEDULER_H

#ifndef WIDGETSSCHEDULER_H
#define WIDGETSSCHEDULER_H

#include <QObject>
#include <QTimer>
#include <ischeduler.h>

class WidgetsScheduler : public QObject, public IScheduler
{
	Q_OBJECT
	Q_INTERFACES(IScheduler)

public:
	Q_INVOKABLE explicit WidgetsScheduler(QObject *parent = nullptr);

public slots:
	void initialize(const QList<Reminder> &allReminders) override;
	bool scheduleReminder(const Reminder &reminder) override;
	void cancleReminder(const QUuid &id) override;

signals:
	void scheduleTriggered(const QUuid &id) final;

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
	int trySchedule(const QDateTime &target);
};

#endif // WIDGETSSCHEDULER_H

#ifndef WIDGETSSCHEDULER_H
#define WIDGETSSCHEDULER_H

#include <QObject>
#include <QTimer>
#include "../RemindMeDaemon/scheduler.h"

class WidgetsScheduler : public QObject, public Scheduler
{
	Q_OBJECT
	Q_INTERFACES(Scheduler)

public:
	explicit WidgetsScheduler(QObject *parent = nullptr);

public slots:
	bool scheduleReminder(const QUuid &id, const QDateTime &timepoint) override;
	void cancleReminder(const QUuid &id) override;

signals:
	void scheduleTriggered(const QUuid &id) final;

protected:
	void timerEvent(QTimerEvent *event) override;

private:
	typedef QPair<QDateTime, int> SchedInfo;
	QHash<QUuid, SchedInfo> _schedules;

	int _loopTimerId;

	void reschedule();
	int trySchedule(const QDateTime &target);
};

#endif // WIDGETSSCHEDULER_H

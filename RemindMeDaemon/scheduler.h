#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QDateTime>
#include <QObject>
#include <QUuid>

class Scheduler
{
public:
	virtual inline ~Scheduler() = default;

public slots:
	virtual bool scheduleReminder(const QUuid &id, const QDateTime &timepoint) = 0;
	virtual void cancleReminder(const QUuid &id) = 0;

signals:
	virtual void scheduleTriggered(const QUuid &id) = 0;
};

#define Scheduler_iid "de.skycoder42.remindme.daemon.Scheduler"
Q_DECLARE_INTERFACE(Scheduler, Scheduler_iid)

#endif // SCHEDULER_H

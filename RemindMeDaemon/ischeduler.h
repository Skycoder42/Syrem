#ifndef ISCHEDULER_H
#define ISCHEDULER_H

#include <QDateTime>
#include <QObject>
#include <QUuid>
#include "reminder.h"

class IScheduler
{
public:
	virtual inline ~IScheduler() = default;

public slots:
	virtual void initialize(const QList<Reminder> &allReminders) = 0;

	virtual bool scheduleReminder(const Reminder &reminder) = 0;
	virtual void cancleReminder(const QUuid &id) = 0;

signals:
	virtual void scheduleTriggered(const QUuid &id) = 0;
};

#define IScheduler_iid "de.skycoder42.remindme.daemon.IScheduler"
Q_DECLARE_INTERFACE(IScheduler, IScheduler_iid)
Q_DECLARE_METATYPE(IScheduler*)

#endif // ISCHEDULER_H

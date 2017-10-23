#ifndef ISCHEDULER_H
#define ISCHEDULER_H

#include <QDateTime>
#include <QObject>
#include <QUuid>

class IScheduler
{
public:
	virtual inline ~IScheduler() = default;

public slots:
	virtual bool scheduleReminder(const QUuid &id, const QDateTime &timepoint) = 0;
	virtual void cancleReminder(const QUuid &id) = 0;

signals:
	virtual void scheduleTriggered(const QUuid &id) = 0;
};

#define IScheduler_iid "de.skycoder42.remindme.daemon.IScheduler"
Q_DECLARE_INTERFACE(IScheduler, IScheduler_iid)

#endif // ISCHEDULER_H
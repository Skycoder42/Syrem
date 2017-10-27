#include "androidscheduler.h"

AndroidScheduler::AndroidScheduler(QObject *parent) :
	QObject(parent),
	IScheduler()
{}

void AndroidScheduler::initialize(const QList<Reminder> &allReminders)
{
}

bool AndroidScheduler::scheduleReminder(const QUuid &id, quint32 versionCode, const QDateTime &timepoint)
{
}

void AndroidScheduler::cancleReminder(const QUuid &id)
{
}

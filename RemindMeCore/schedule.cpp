#include "schedule.h"

Schedule::Schedule(QObject *parent) :
	QObject(parent)
{}

bool Schedule::isLoop()
{
	return false;
}



OneTimeSchedule::OneTimeSchedule(QObject *parent) :
	OneTimeSchedule({}, parent)
{}

OneTimeSchedule::OneTimeSchedule(const QDateTime timepoint, QObject *parent) :
	Schedule(parent),
	timepoint(timepoint)
{}

QDateTime OneTimeSchedule::nextSchedule(const QDateTime &since)
{
	if(since <= timepoint)
		return timepoint;
	else
		return {};
}



LoopSchedule::LoopSchedule(QObject *parent) :
	Schedule(parent)
{}

bool LoopSchedule::isLoop()
{
	return true;
}

QDateTime LoopSchedule::nextSchedule(const QDateTime &since)
{
	Q_UNIMPLEMENTED();
	return {};
}

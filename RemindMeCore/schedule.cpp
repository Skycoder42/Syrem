#include "schedule.h"

Schedule::Schedule(QObject *parent) :
	QObject(parent)
{}



OneTimeSchedule::OneTimeSchedule(QObject *parent) :
	OneTimeSchedule({}, parent)
{}

OneTimeSchedule::OneTimeSchedule(const QDateTime timepoint, QObject *parent) :
	Schedule(parent),
	timepoint(timepoint)
{}

bool OneTimeSchedule::isRepeating() const
{
	return false;
}

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

bool LoopSchedule::isRepeating() const
{
	return true;
}

QDateTime LoopSchedule::nextSchedule(const QDateTime &since)
{
	Q_UNIMPLEMENTED();
	return {};
}



MultiSchedule::MultiSchedule(QObject *parent) :
	Schedule(parent),
	subSchedules()
{}

void MultiSchedule::addSubSchedule(Schedule *schedule)
{
	Q_ASSERT_X(schedule, Q_FUNC_INFO, "schedule must not be null");
	schedule->setParent(this);
	subSchedules.append(schedule);
}

bool MultiSchedule::isRepeating() const
{
	return true;
}

QDateTime MultiSchedule::nextSchedule(const QDateTime &since)
{
	QDateTime closest;
	foreach(auto schedule, subSchedules) {
		auto next = schedule->nextSchedule(since);
		if(next.isValid()) {
			if(!closest.isValid() || closest > next)
				closest = next;
		}
	}
	return closest;
}

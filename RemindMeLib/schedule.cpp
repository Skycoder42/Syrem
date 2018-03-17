#include "schedule.h"
#include "dateparser.h"

Schedule::Schedule(QObject *parent) :
	QObject(parent),
	_current()
{}

Schedule::Schedule(const QDateTime &since, QObject *parent) :
	QObject(parent),
	_current(since)
{}

QDateTime Schedule::current() const
{
	return _current;
}

QDateTime Schedule::nextSchedule()
{
	_current = generateNextSchedule();
	emit currentChanged(_current);
	return _current;
}



OneTimeSchedule::OneTimeSchedule(QObject *parent) :
	OneTimeSchedule({}, {}, parent)
{}

OneTimeSchedule::OneTimeSchedule(const QDateTime timepoint, const QDateTime &since, QObject *parent) :
	Schedule(since, parent),
	timepoint(timepoint)
{}

bool OneTimeSchedule::isRepeating() const
{
	return false;
}

QDateTime OneTimeSchedule::generateNextSchedule()
{
	if(current() < timepoint)
		return timepoint;
	else
		return {};
}



LoopSchedule::LoopSchedule(QObject *parent) :
	LoopSchedule({}, parent)
{}

LoopSchedule::LoopSchedule(const QDateTime &since, QObject *parent) :
	Schedule(since, parent),
	type(nullptr),
	datum(nullptr),
	time(),
	from(),
	until()
{}

bool LoopSchedule::isRepeating() const
{
	return true;
}

QDateTime LoopSchedule::generateNextSchedule()
{
	QDateTime tp;
	if(from.isValid() && current() < from)
		tp = from;
	else
		tp = current();

	tp = type->nextDateTime(tp);

	if(datum)
		tp.setDate(datum->nextDate(tp.date(), true));
	if(time.isValid())
		tp.setTime(time);

	if(until.isValid() && tp > until)
		return {};
	else
		return tp;
}



MultiSchedule::MultiSchedule(QObject *parent) :
	Schedule(parent),
	subSchedules()
{}

MultiSchedule::MultiSchedule(const QDateTime &since, QObject *parent) :
	Schedule(since, parent),
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

QDateTime MultiSchedule::generateNextSchedule()
{
	QDateTime closest;
	foreach(auto schedule, subSchedules) {
		auto next = schedule->current();
		while(next.isValid() && next <= current())
			next = schedule->nextSchedule();

		if(next.isValid()) {
			if(!closest.isValid() || closest > next)
				closest = next;
		}
	}
	return closest;
}

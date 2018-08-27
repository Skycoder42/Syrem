#include "schedule.h"
#include "dateparser.h"
using namespace Expressions;

Schedule::Schedule(QObject *parent) :
	QObject(parent),
	_current()
{}

Schedule::Schedule(QDateTime since, QObject *parent) :
	QObject{parent},
	_current{std::move(since)}
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



SingularSchedule::SingularSchedule(QObject *parent) :
	Schedule{parent}
{}

SingularSchedule::SingularSchedule(QDateTime timepoint, QObject *parent) :
	Schedule{std::move(timepoint), parent}
{}

bool SingularSchedule::isRepeating() const
{
	return false;
}

QDateTime SingularSchedule::generateNextSchedule()
{
	return {}; // has no next schedule
}



RepeatedSchedule::RepeatedSchedule(QObject *parent):
	Schedule{parent}
{}

RepeatedSchedule::RepeatedSchedule(Expressions::Term loopTerm, Expressions::Term fenceTerm, QDateTime from, QDateTime until, QObject *parent) :
	Schedule{std::move(from), parent},
	loopTerm{std::move(loopTerm)},
	fenceTerm{std::move(fenceTerm)},
	until{std::move(until)}
{}

bool RepeatedSchedule::isRepeating() const
{
	return true;
}

QDateTime RepeatedSchedule::generateNextSchedule()
{
	const auto last = current();
	auto next = last;

	// check if the initial fences need to be generated
	// this is only the case on the very first run
	// Use the "last" aka "from" time as reference to generate the first fence in that case
	auto applyFenced = false; // normally, we are not fenced
	QDateTime fenceBegin;
	if(!fenceTerm.isEmpty() && !fenceEnd.isValid()) {
		fenceBegin = generateFences(last);
		next = fenceBegin;
		applyFenced = true; // Apply fenced, is the first term in the fence
	}

	// get the next time on the schedule
	next = loopTerm.apply(next, applyFenced);
	// and "fix" it in case it goes below the fence (can happen for weeks)

	// if it exceeds the fence, generate a new fence
	if(!fenceTerm.isEmpty() && next >= fenceEnd) {
		const auto nextFence = generateFences(fenceEnd); //use end of last fence as reference
		if(next >= fenceEnd) // can happen for absolute fences
			return {};
		// Generate the next real sched. based of the nextFence. Only regen if not already within the new fence
		if(next < nextFence)
			next = loopTerm.apply(nextFence, true); // Apply fenced, is the first term in the fence
		// safeguard for potential edge cases
		if(next >= fenceEnd)
			return {};
	}

	// safeguard for potential edge cases
	if(next <= last)
		return {};
	// verify if below until limit
	if(until.isValid() && next > until)
		return {};
	else
		return next;
}

QDateTime RepeatedSchedule::generateFences(const QDateTime &current)
{
	// get the next fence begin from right after the current "end"
	auto fenceBegin = fenceTerm.apply(current);
	if(!fenceTerm.hasTimeScope()) // reset time to midnight if not part of the fence
		fenceBegin.setTime(QTime{0, 0});

	// find the smallest scope that is contained by the fence term
	for(int s = SubTerm::Minute; s <= SubTerm::Year; s = (s << 1)) {
		if(fenceTerm.scope().testFlag(static_cast<SubTerm::ScopeFlag>(s))) {
			using namespace std::chrono;
			// when found: add "time" to fence begin to leave that scope
			switch(static_cast<SubTerm::ScopeFlag>(s)) {
			case SubTerm::Minute:
				fenceEnd = fenceBegin.addSecs(duration_cast<seconds>(minutes{1}).count());
				break;
			case SubTerm::Hour:
				fenceEnd = fenceBegin.addSecs(duration_cast<seconds>(hours{1}).count());
				break;
			case SubTerm::Day:
				fenceEnd = fenceBegin.addDays(1);
				break;
			case SubTerm::Week:
				fenceEnd = fenceBegin.addDays(7);
				break;
			case SubTerm::Month:
				fenceEnd = fenceBegin.addMonths(1);
				break;
			case SubTerm::Year:
				fenceEnd = fenceBegin.addYears(1);
				break;
			default:
				Q_UNREACHABLE();
				break;
			}
			return fenceBegin;
		}
	}

	Q_UNREACHABLE();
	return {};
}



MultiSchedule::MultiSchedule(QObject *parent) :
	Schedule{parent}
{}

MultiSchedule::MultiSchedule(QDateTime since, QObject *parent) :
	Schedule{std::move(since), parent}
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
	for(auto schedule : qAsConst(subSchedules)) {
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



// ------------- Historic Schedules -------------



OneTimeSchedule::OneTimeSchedule(QObject *parent) :
	OneTimeSchedule({}, {}, parent)
{}

OneTimeSchedule::OneTimeSchedule(QDateTime timepoint, QDateTime since, QObject *parent) :
	Schedule{std::move(since), parent},
	timepoint{std::move(timepoint)}
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
	LoopSchedule{{}, parent}
{}

LoopSchedule::LoopSchedule(QDateTime since, QObject *parent) :
	Schedule{std::move(since), parent}
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

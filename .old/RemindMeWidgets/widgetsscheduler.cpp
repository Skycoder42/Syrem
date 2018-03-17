#include "widgetsscheduler.h"
#include <QDebug>
#include <QEvent>
#include <chrono>
using namespace std::chrono;

WidgetsScheduler::WidgetsScheduler(QObject *parent) :
	QObject(parent),
	IScheduler(),
	_schedules(),
	_loopTimerId(startTimer(minutes(50), Qt::VeryCoarseTimer))
{
	if(_loopTimerId == 0)
		qCritical() << "Failed to start main timer loop";
}

void WidgetsScheduler::initialize(const QList<Reminder> &allReminders)
{
	foreach (auto sched, _schedules) {
		if(sched.timerId != 0)
			killTimer(sched.timerId);
	}
	_schedules.clear();

	foreach(auto rem, allReminders)
		scheduleReminder(rem);
}

bool WidgetsScheduler::scheduleReminder(const Reminder &reminder)
{
	if(!reminder.current().isValid())
		return false;

	auto current = _schedules.value(reminder.id());
	if(current.date.isValid() && current.version == reminder.versionCode())
		return false;//already scheduled

	cancleReminder(reminder.id());
	auto tId = trySchedule(reminder.current());
	if(tId == -1)
		emit scheduleTriggered(reminder.id());
	else
		_schedules.insert(reminder.id(), {reminder.versionCode(), reminder.current(), tId});
	return true;
}

void WidgetsScheduler::cancleReminder(const QUuid &id)
{
	auto tInfo = _schedules.take(id);
	if(tInfo.date.isValid() && tInfo.timerId != 0)
		killTimer(tInfo.timerId);
}


void WidgetsScheduler::timerEvent(QTimerEvent *event)
{
	auto timerId = event->timerId();
	if(timerId == _loopTimerId)
		reschedule();
	else {
		for(auto it = _schedules.begin(); it != _schedules.end(); it++) {
			if(it->timerId == timerId) {
				killTimer(timerId);

				auto tDiff = qAbs(it->date.secsTo(QDateTime::currentDateTime()));
				if(tDiff > 60)
					qWarning() << "Timer triggered with great target time difference of" << tDiff << "seconds";

				emit scheduleTriggered(it.key());
				_schedules.erase(it);
				return;
			}
		}

		qWarning() << "Timer triggered, but no schedule matches the ID";
		killTimer(timerId);
	}
}

void WidgetsScheduler::reschedule()
{
	for(auto it = _schedules.begin(); it != _schedules.end();) {
		if(it->timerId == 0) {
			it->timerId = trySchedule(it->date);
			if(it->timerId == -1) {//trigger now, as it's in the past
				emit scheduleTriggered(it.key());
				it = _schedules.erase(it);
				continue;
			}
		}
		it++;
	}
}

int WidgetsScheduler::trySchedule(const QDateTime &target)
{
	auto secs = QDateTime::currentDateTime().secsTo(target);
	if(secs <= 0)
		return -1;
	else if(seconds(secs) < hours(1)) { //only schedule "close" events
		auto id = startTimer(duration_cast<milliseconds>(seconds(secs)).count(), Qt::VeryCoarseTimer);
		if(id == 0)
			qCritical() << "Failed to start timer for schedule at" << target;
		return id;
	} else
		return 0;
}

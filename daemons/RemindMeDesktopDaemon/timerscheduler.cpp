#include "timerscheduler.h"
#include <QDebug>
#include <QEvent>
#include <QLoggingCategory>
#include <chrono>
using namespace std::chrono;

Q_LOGGING_CATEGORY(scheduler, "scheduler")

TimerScheduler::TimerScheduler(QObject *parent) :
	QObject(parent),
	_schedules(),
	_loopTimerId(startTimer(minutes(50), Qt::VeryCoarseTimer))
{
	if(_loopTimerId == 0)
		qCCritical(scheduler) << "Failed to start main timer loop";
}

void TimerScheduler::initialize(const QList<Reminder> &allReminders)
{
	for(const auto& rem : allReminders)
		scheduleReminder(rem);
}

void TimerScheduler::scheduleReminder(const Reminder &reminder)
{
	if(!reminder.current().isValid())
		return;

	auto current = _schedules.value(reminder.id());
	if(current.date.isValid() && current.version == reminder.versionCode()) {
		qCDebug(scheduler) << "Reminder with id" << reminder.id() << "already scheduled";
		return;
	}

	cancleReminder(reminder.id());
	auto tId = trySchedule(reminder.current(), reminder.id());
	if(tId == -1)
		emit scheduleTriggered(reminder.id());
	else
		_schedules.insert(reminder.id(), {reminder.versionCode(), reminder.current(), tId});
	return;
}

void TimerScheduler::cancleReminder(QUuid id)
{
	auto tInfo = _schedules.take(id);
	if(tInfo.date.isValid() && tInfo.timerId != 0) {
		qCDebug(scheduler) << "Canceled timer for reminder with id" << id;
		killTimer(tInfo.timerId);
	}
}

void TimerScheduler::cancelAll()
{
	for(const auto &sched : qAsConst(_schedules))
		killTimer(sched.timerId);
	_schedules.clear();
	qCDebug(scheduler) << "Cleared all active schedules";
}


void TimerScheduler::timerEvent(QTimerEvent *event)
{
	auto timerId = event->timerId();
	if(timerId == _loopTimerId)
		reschedule();
	else {
		for(auto it = _schedules.begin(); it != _schedules.end(); it++) {
			if(it->timerId == timerId) {
				killTimer(timerId);

				auto tDiff = qAbs(it->date.secsTo(QDateTime::currentDateTime()));
				if(tDiff > 60) {
					qCWarning(scheduler) << "Timer triggered with great target time difference of" << tDiff
										 << "seconds for reminder with id" << it.key();
				} else
					qCInfo(scheduler) << "Timer triggered for reminder with id" << it.key();

				emit scheduleTriggered(it.key());
				_schedules.erase(it);
				return;
			}
		}

		qCWarning(scheduler) << "Timer triggered, but no schedule matches the ID";
		killTimer(timerId);
	}
}

void TimerScheduler::reschedule()
{
	qCDebug(scheduler) << "Rescheduling timers for the near future";
	for(auto it = _schedules.begin(); it != _schedules.end();) {
		if(it->timerId == 0) {
			it->timerId = trySchedule(it->date, it.key());
			if(it->timerId == -1) {//trigger now, as it's in the past
				emit scheduleTriggered(it.key());
				it = _schedules.erase(it);
				continue;
			}
		}
		it++;
	}
}

int TimerScheduler::trySchedule(const QDateTime &target, QUuid id)
{
	auto secs = QDateTime::currentDateTime().secsTo(target);
	if(secs <= 0) {
		qCInfo(scheduler) << "Immediatly triggering scheduled reminder with id" << id << "due to overtime" ;
		return -1;
	}
	else if(seconds(secs) < hours(1)) { //only schedule "close" events
		qCInfo(scheduler) << "Scheduling near future reminder" << id << "for in" << secs << "seconds";
		auto id = startTimer(duration_cast<milliseconds>(seconds(secs)).count(), Qt::VeryCoarseTimer);
		if(id == 0)
			qCCritical(scheduler) << "Failed to start timer for reminder" << id << "at" << target;
		return id;
	} else {
		qCDebug(scheduler) << "Postponing scheduling for" << id << "as it is in the far future";
		return 0;
	}
}

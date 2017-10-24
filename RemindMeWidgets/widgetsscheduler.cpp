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

bool WidgetsScheduler::scheduleReminder(const QUuid &id, const QDateTime &timepoint)
{
	if(!timepoint.isValid())
		return false;

	cancleReminder(id);
	auto tId = trySchedule(timepoint);
	if(tId == -1)
		emit scheduleTriggered(id);
	else
		_schedules.insert(id, {timepoint, tId});
	return true;
}

void WidgetsScheduler::cancleReminder(const QUuid &id)
{
	auto tInfo = _schedules.take(id);
	if(tInfo.first.isValid() && tInfo.second != 0)
		killTimer(tInfo.second);
}


void WidgetsScheduler::timerEvent(QTimerEvent *event)
{
	auto timerId = event->timerId();
	if(timerId == _loopTimerId)
		reschedule();
	else {
		for(auto it = _schedules.begin(); it != _schedules.end(); it++) {
			if(it->second == timerId) {
				killTimer(timerId);

				auto tDiff = qAbs(it->first.secsTo(QDateTime::currentDateTime()));
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
		if(it->second == 0) {
			it->second = trySchedule(it->first);
			if(it->second == -1) {//trigger now, as it's in the past
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

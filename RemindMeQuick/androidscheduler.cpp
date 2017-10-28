#include "androidscheduler.h"
#include <QtAndroid>
#include <chrono>

AndroidScheduler::AndroidScheduler(QObject *parent) :
	QObject(parent),
	IScheduler(),
	_settings(new QSettings(this)),
	_autoSync(true)
{
	_settings->beginGroup(QStringLiteral("scheduler"));
}

void AndroidScheduler::initialize(const QList<Reminder> &allReminders)
{
	_autoSync = false;

	auto currentKeys = _settings->childKeys();
	foreach(auto rem, allReminders) {
		scheduleReminder(rem);
		currentKeys.removeOne(rem.id().toString());
	}
	foreach(auto oldKey, currentKeys) {
		if(oldKey == QStringLiteral("cnt"))
			continue;
		QUuid oldId(oldKey);
		if(!oldId.isNull())
			cancleReminder(oldKey);
	}

	_settings->sync();
	_autoSync = true;
}

bool AndroidScheduler::scheduleReminder(const Reminder &reminder)
{
	using namespace std::chrono;
	if(!reminder.current().isValid())
		return false;

	//adjust the trigger time (+2/5 min to give desktop the change to clean up first)
	auto remKey = reminder.id().toString();
	if(reminder.current() <= QDateTime::currentDateTime()) {
		cancleReminder(reminder.id());//better save then sorry -> cancle if already scheduled
		emit scheduleTriggered(reminder.id());
		return true;
	}
	auto trigger = reminder.current();
	if(reminder.isImportant())
		trigger = trigger.addSecs(duration_cast<seconds>(minutes(2)).count());
	else
		trigger = trigger.addSecs(duration_cast<seconds>(minutes(5)).count());

	//get alarm id
	int alarmId = 0;
	if(!_settings->contains(remKey)) {
		alarmId = _settings->value(QStringLiteral("cnt"), 0).toInt() + 1;
		_settings->setValue(QStringLiteral("cnt"), alarmId);
		_settings->setValue(remKey, alarmId);
		if(_autoSync)
			_settings->sync();
	} else
		alarmId = _settings->value(remKey).toInt();

	auto service = QtAndroid::androidService();
	service.callMethod<void>("createSchedule", "(IZJ)V",
							 (jint)alarmId,
							 (jboolean)reminder.isImportant(),
							 (jlong)trigger.toMSecsSinceEpoch());
	return true;
}

void AndroidScheduler::cancleReminder(const QUuid &id)
{
	auto remKey = id.toString();
	if(_settings->contains(remKey)) {
		auto alarmId = _settings->value(remKey).toInt();

		auto service = QtAndroid::androidService();
		service.callMethod<void>("cancleSchedule", "(I)V", (jint)alarmId);

		_settings->remove(remKey);
		if(_autoSync)
			_settings->sync();
	}
}

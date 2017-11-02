#include "androidscheduler.h"
#include <QtAndroid>
#include <chrono>
#include <registry.h>

AndroidScheduler::AndroidScheduler(QObject *parent) :
	QObject(parent),
	IScheduler(),
	_settings(new QSettings(this)),
	_autoSync(true)
{
	_settings->beginGroup(QStringLiteral("scheduler"));
}

void AndroidScheduler::triggerSchedule(const QUuid &id, quint32 versionCode)
{
	if(id.isNull())
		return;
	auto self = qobject_cast<AndroidScheduler*>(Registry::acquireObject(IScheduler_iid));
	if(self)
		self->performTrigger(id, versionCode);
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

	//save the used versionCode
	_settings->setValue(remKey, reminder.versionCode());
	if(_autoSync)
		_settings->sync();

	auto service = QtAndroid::androidService();
	service.callMethod<void>("createSchedule", "(Ljava/lang/String;IZJ)V",
							 QAndroidJniObject::fromString(remKey).object<jstring>(),
							 (jint)reminder.versionCode(),
							 (jboolean)reminder.isImportant(),
							 (jlong)trigger.toMSecsSinceEpoch());
	return true;
}

void AndroidScheduler::cancleReminder(const QUuid &id)
{
	auto remKey = id.toString();
	auto service = QtAndroid::androidService();
	service.callMethod<void>("cancelSchedule", "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(remKey).object<jstring>());

	if(_settings->contains(remKey)) {
		_settings->remove(remKey);
		if(_autoSync)
			_settings->sync();
	}
}

void AndroidScheduler::performTrigger(const QUuid &id, quint32 versionCode)
{
	auto remKey = id.toString();
	if(_settings->contains(remKey)) {
		auto vCode = _settings->value(remKey).toUInt();
		if(vCode == versionCode) {
			_settings->remove(remKey);
			if(_autoSync)
				_settings->sync();
			emit scheduleTriggered(id);
		}
	}
}

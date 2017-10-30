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
	//TODO add boot receiver
	_settings->beginGroup(QStringLiteral("scheduler"));
}

void AndroidScheduler::triggerSchedule(const QString &id)
{
	auto data = Reminder::readUniqueString(id);
	if(data.first.isNull())
		return;
	auto self = qobject_cast<AndroidScheduler*>(Registry::acquireObject(IScheduler_iid));
	if(self)
		self->performTrigger(data.first, data.second);
}

void AndroidScheduler::initialize(const QList<Reminder> &allReminders)
{
	_autoSync = false;

	auto currentKeys = _settings->childGroups();
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
	if(!_settings->childGroups().contains(remKey)) {
		alarmId = _settings->value(QStringLiteral("cnt"), 0).toInt() + 1;
		_settings->setValue(QStringLiteral("cnt"), alarmId);
		_settings->beginGroup(remKey);
		_settings->setValue(QStringLiteral("version"), reminder.versionCode());
		_settings->setValue(QStringLiteral("id"), alarmId);
		_settings->endGroup();
		if(_autoSync)
			_settings->sync();
	} else {
		_settings->beginGroup(remKey);
		alarmId = _settings->value(QStringLiteral("id")).toInt();
		_settings->setValue(QStringLiteral("version"), reminder.versionCode());
		_settings->endGroup();
		if(_autoSync)
			_settings->sync();
	}

	auto service = QtAndroid::androidService();
	service.callMethod<void>("createSchedule", "(IZJLjava/lang/String;)V",
							 (jint)alarmId,
							 (jboolean)reminder.isImportant(),
							 (jlong)trigger.toMSecsSinceEpoch(),
							 QAndroidJniObject::fromString(reminder.uniqueString()).object<jstring>());
	return true;
}

void AndroidScheduler::cancleReminder(const QUuid &id)
{
	auto remKey = id.toString();
	auto idKey = QStringLiteral("%1/id").arg(remKey);
	if(_settings->contains(idKey)) {
		auto alarmId = _settings->value(idKey).toInt();

		auto service = QtAndroid::androidService();
		service.callMethod<void>("cancelSchedule", "(I)V", (jint)alarmId);

		_settings->remove(remKey);
		if(_autoSync)
			_settings->sync();
	}
}

void AndroidScheduler::performTrigger(const QUuid &id, quint32 versionCode)
{
	auto remKey = id.toString();
	if(_settings->childGroups().contains(remKey)) {
		auto vCode = _settings->value(QStringLiteral("%1/version").arg(remKey)).toUInt();
		if(vCode == versionCode) {
			_settings->remove(remKey);
			if(_autoSync)
				_settings->sync();
			emit scheduleTriggered(id);
		}
	}
}

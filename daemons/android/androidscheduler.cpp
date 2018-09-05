#include "androidscheduler.h"
#include <QtAndroid>
#include <QtMvvmCore/ServiceRegistry>
#include <QAndroidJniExceptionCleaner>
#include <chrono>
#include <syncedsettings.h>

AndroidScheduler::AndroidScheduler(QObject *parent) :
	QObject(parent),
	_jScheduler("de/skycoder42/syrem/Scheduler", "(Landroid/content/Context;)V",
				QtAndroid::androidService().object())
{}

void AndroidScheduler::setupAutoCheck(int interval)
{
	_jScheduler.callMethod<void>("scheduleAutoCheck", "(I)V",
								 static_cast<jint>(interval));
}

bool AndroidScheduler::scheduleReminder(const Reminder &reminder)
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};

	using namespace std::chrono;
	if(!reminder.current().isValid())
		return true;

	//adjust the trigger time (+2/5 min to give desktop the change to clean up first)
	auto remKey = reminder.id().toString();
	if(reminder.current() <= QDateTime::currentDateTime()) {
		cancleReminder(reminder.id());//better save then sorry -> cancle if already scheduled
		return false; //only return false here to indicate the reminder is already triggered
	}
	auto trigger = reminder.current();
	int delay = 0;
	if(reminder.isImportant())
		delay = SyncedSettings::instance()->scheduler.delay.important;
	else
		delay = SyncedSettings::instance()->scheduler.delay.normal;
	if(delay > 0)
		trigger = trigger.addSecs(duration_cast<seconds>(minutes(delay)).count());

	_jScheduler.callMethod<void>("createSchedule", "(Ljava/lang/String;IZJ)V",
								 QAndroidJniObject::fromString(remKey).object<jstring>(),
								 (jint)reminder.versionCode(),
								 (jboolean)reminder.isImportant(),
								 (jlong)trigger.toMSecsSinceEpoch());
	return true;
}

void AndroidScheduler::cancleReminder(const QUuid &id)
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	_jScheduler.callMethod<void>("cancelSchedule", "(Ljava/lang/String;)V",
								 QAndroidJniObject::fromString(id.toString()).object<jstring>());
}

#include "androidnotifier.h"
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <syncedsettings.h>

AndroidNotifier::AndroidNotifier(QObject *parent) :
	QObject(parent),
	_jNotifier("de/skycoder42/remindme/Notifier", "(Landroid/content/Context;)V",
			   QtAndroid::androidService().object())
{}

void AndroidNotifier::showNotification(const Reminder &reminder)
{
	//build the choices
	QAndroidJniEnvironment env;
	SnoozeTimes times = SyncedSettings::instance()->scheduler.snoozetimes;
	auto choices = QAndroidJniObject::fromLocalRef(env->NewObjectArray((jint)times.size(), env->FindClass("java/lang/String"), NULL));
	for(auto i = 0; i < times.size(); i++) {
		auto str = QAndroidJniObject::fromString(times.value(i));
		env->SetObjectArrayElement(choices.object<jobjectArray>(), i, str.object());
	}

	_jNotifier.callMethod<void>("notify", "(Ljava/lang/String;IZLjava/lang/String;[Ljava/lang/String;)V",
								QAndroidJniObject::fromString(reminder.id().toString()).object(),
								(jint)reminder.versionCode(),
								(jboolean)reminder.isImportant(),
								QAndroidJniObject::fromString(reminder.description()).object(),
								choices.object());
}

void AndroidNotifier::removeNotification(const QUuid &id)
{
	_jNotifier.callMethod<void>("cancel", "(Ljava/lang/String;)V",
								QAndroidJniObject::fromString(id.toString()).object());
}

void AndroidNotifier::showErrorMessage(const QString &error)
{
	_jNotifier.callMethod<void>("notifyError", "(Ljava/lang/String;)V",
								QAndroidJniObject::fromString(error).object());
}

#include "androidnotifier.h"
#include <QtAndroid>
#include <syncedsettings.h>

AndroidNotifier::AndroidNotifier(QObject *parent) :
	QObject(parent),
	_jNotifier("de/skycoder42/syrem/Notifier", "(Landroid/content/Context;)V",
			   QtAndroid::androidService().object())
{}

void AndroidNotifier::showNotification(const Reminder &reminder)
{
	//build the choices
	QAndroidJniEnvironment env;
	auto choices = createSnoozeArray(env);

	_jNotifier.callMethod<void>("notify", "(Ljava/lang/String;IZLjava/lang/CharSequence;[Ljava/lang/String;)V",
								QAndroidJniObject::fromString(reminder.id().toString()).object(),
								(jint)reminder.versionCode(),
								(jboolean)reminder.isImportant(),
								QAndroidJniObject::fromString(reminder.description()).object(),
								choices.object());
}

void AndroidNotifier::showParserError(const Reminder &reminder, const QString &errorText)
{
	//build the choices
	QAndroidJniEnvironment env;
	auto choices = createSnoozeArray(env);
	auto text = tr("<p>%1</p>%2")
				.arg(reminder.description().toHtmlEscaped())
				.arg(errorText);
	auto htmlText = QAndroidJniObject::callStaticObjectMethod("android/text/Html", "fromHtml",
															  "(Ljava/lang/String;I)Landroid/text/Spanned;",
															  QAndroidJniObject::fromString(text).object(),
															  (jint)0);

	_jNotifier.callMethod<void>("notify", "(Ljava/lang/String;IZLjava/lang/CharSequence;[Ljava/lang/String;Z)V",
								QAndroidJniObject::fromString(reminder.id().toString()).object(),
								(jint)reminder.versionCode(),
								(jboolean)reminder.isImportant(),
								htmlText.object(),
								choices.object(),
								(jboolean)true);
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

void AndroidNotifier::cleanNotifications()
{
	_jNotifier.callMethod<void>("cancelAll");
}

QAndroidJniObject AndroidNotifier::createSnoozeArray(QAndroidJniEnvironment &env)
{
	SnoozeTimes times = SyncedSettings::instance()->scheduler.snoozetimes;
	auto choices = QAndroidJniObject::fromLocalRef(env->NewObjectArray((jint)times.size(), env->FindClass("java/lang/String"), NULL));
	for(auto i = 0; i < times.size(); i++) {
		auto str = QAndroidJniObject::fromString(times.value(i));
		env->SetObjectArrayElement(choices.object<jobjectArray>(), i, str.object());
	}
	return choices;
}

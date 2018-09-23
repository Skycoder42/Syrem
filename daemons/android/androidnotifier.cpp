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
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	auto choices = createSnoozeArray(env);

	_jNotifier.callMethod<void>("notify", "(Ljava/lang/String;IZLjava/lang/CharSequence;Z[Ljava/lang/String;)V",
								QAndroidJniObject::fromString(reminder.id().toString()).object(),
								static_cast<jint>(reminder.versionCode()),
								static_cast<jboolean>(reminder.isImportant()),
								toAndroidHtml(reminder.htmlDescription()).object(),
								static_cast<jboolean>(reminder.hasUrls()),
								choices.object());
}

void AndroidNotifier::showParserError(const Reminder &reminder, const QString &errorText)
{
	//build the choices
	QAndroidJniEnvironment env;
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};

	auto choices = createSnoozeArray(env);
	auto text = tr("<p>%1</p>%2")
				.arg(reminder.htmlDescription())
				.arg(errorText);
	_jNotifier.callMethod<void>("notify", "(Ljava/lang/String;IZLjava/lang/CharSequence;Z[Ljava/lang/String;Z)V",
								QAndroidJniObject::fromString(reminder.id().toString()).object(),
								static_cast<jint>(reminder.versionCode()),
								static_cast<jboolean>(reminder.isImportant()),
								toAndroidHtml(text).object(),
								static_cast<jboolean>(reminder.hasUrls()),
								choices.object(),
								static_cast<jboolean>(true));
}

void AndroidNotifier::removeNotification(const QUuid &id)
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	_jNotifier.callMethod<void>("cancel", "(Ljava/lang/String;)V",
								QAndroidJniObject::fromString(id.toString()).object());
}

void AndroidNotifier::showErrorMessage(const QString &error)
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	_jNotifier.callMethod<void>("notifyError", "(Ljava/lang/String;)V",
								QAndroidJniObject::fromString(error).object());
}

void AndroidNotifier::cleanNotifications()
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	_jNotifier.callMethod<void>("cancelAll");
}

QAndroidJniObject AndroidNotifier::createSnoozeArray(QAndroidJniEnvironment &env)
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	SnoozeTimes times = SyncedSettings::instance()->scheduler.snoozetimes;
	auto choices = QAndroidJniObject::fromLocalRef(env->NewObjectArray(static_cast<jint>(times.size()), env->FindClass("java/lang/String"), nullptr));
	for(auto i = 0; i < times.size(); i++) {
		auto str = QAndroidJniObject::fromString(times.value(i));
		env->SetObjectArrayElement(choices.object<jobjectArray>(), i, str.object());
	}
	return choices;
}

QAndroidJniObject AndroidNotifier::toAndroidHtml(const QString &text) const
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	return QAndroidJniObject::callStaticObjectMethod("android/text/Html", "fromHtml", //MAJOR deprecated in API-Level 24
													 "(Ljava/lang/String;)Landroid/text/Spanned;",
													 QAndroidJniObject::fromString(text).object(),
													 static_cast<jint>(0));
}

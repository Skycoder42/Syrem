#include "androidnotifier.h"
#include <QtAndroid>

AndroidNotifier::AndroidNotifier(QObject *parent) :
	QObject(parent),
	INotifier()
{}

void AndroidNotifier::beginSetup()
{
}

void AndroidNotifier::endSetup()
{
}

void AndroidNotifier::showNotification(const Reminder &reminder)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("notify", "(Ljava/lang/String;ZLjava/lang/String;)V",
							 QAndroidJniObject::fromString(reminder.id().toString()).object(),
							 (jboolean)reminder.isImportant(),
							 QAndroidJniObject::fromString(reminder.description()).object());
}

void AndroidNotifier::removeNotification(const QUuid &id)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("cancelNotify", "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(id.toString()).object());
}

void AndroidNotifier::showErrorMessage(const QString &error)
{
}

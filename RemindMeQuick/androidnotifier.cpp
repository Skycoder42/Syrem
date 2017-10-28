#include "androidnotifier.h"
#include <QtAndroid>
#include <QAndroidJniEnvironment>

AndroidNotifier::AndroidNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_setup(false),
	_setupIds()
{}

void AndroidNotifier::beginSetup()
{
	_setupIds.clear();
	_setup = true;
}

void AndroidNotifier::endSetup()
{
	_setup = false;

	QList<QUuid> cancelList;

	QAndroidJniEnvironment env;
	auto service = QtAndroid::androidService();
	auto keys = service.callObjectMethod("activeNotifications", "()[Ljava/lang/String;");
	if(!keys.isValid() || keys.object() == nullptr)
		return;

	auto cnt = env->GetArrayLength(keys.object<jobjectArray>());
	for(auto i = 0; i < cnt; i++) {
		QAndroidJniObject obj(env->GetObjectArrayElement(keys.object<jobjectArray>(), i));
		QUuid keyId(obj.toString());
		if(!keyId.isNull() && !_setupIds.contains(keyId))
			cancelList.append(keyId);
	}

	_setupIds.clear();
	foreach(auto key, cancelList)
		removeNotification(key);
}

void AndroidNotifier::showNotification(const Reminder &reminder)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("notify", "(Ljava/lang/String;ZLjava/lang/String;)V",
							 QAndroidJniObject::fromString(reminder.id().toString()).object(),
							 (jboolean)reminder.isImportant(),
							 QAndroidJniObject::fromString(reminder.description()).object());
	if(_setup)
		_setupIds.append(reminder.id());
}

void AndroidNotifier::removeNotification(const QUuid &id)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("cancelNotify", "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(id.toString()).object());
	if(_setup)
		_setupIds.removeOne(id);
}

void AndroidNotifier::showErrorMessage(const QString &error)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("notifyError", "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(error).object());
}

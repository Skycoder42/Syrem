#include "androidnotifier.h"
#include "androidscheduler.h"
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <registry.h>

bool AndroidNotifier::_canInvoke = false;
QMutex AndroidNotifier::_invokeMutex;
QList<QPair<QString, QString>> AndroidNotifier::_startServiceCache;

AndroidNotifier::AndroidNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_setup(false),
	_setupIds()
{}

void AndroidNotifier::initIntent(const QString &action, const QString &data)
{
	QMutexLocker _(&_invokeMutex);

	_startServiceCache.append({action, data});
	if(_canInvoke) {
		auto obj = Registry::acquireObject(INotifier_iid);
		QMetaObject::invokeMethod(obj, "reactToStart", Qt::QueuedConnection);
	}
}

//TODO how-to-actions:
// static c++ callback from java (+ init var) -> waits until endSetup / or not if already setup done
// block id + versionCode from beeing notified
// the laods the reminder and emits the correct signal
// triggerSync and stopService once done

// for custom snooze -> open GUI instead
// in gui, call startService with another custom intend, INCLUDING the calculated time
// in svc, do the same as usual

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

	_canInvoke = true;
	if(!_startServiceCache.isEmpty())
		QMetaObject::invokeMethod(this, "reactToStart", Qt::QueuedConnection);
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

void AndroidNotifier::reactToStart()
{
	QMutexLocker _(&_invokeMutex);

	auto shouldQuit = true;
	foreach(auto entry, _startServiceCache) {
		if(entry.first == QStringLiteral("de.skycoder42.remindme.ActionScheduler"))
			AndroidScheduler::triggerSchedule(entry.second);
	}
	_startServiceCache.clear();

	if(shouldQuit) {
		auto service = QtAndroid::androidService();
		service.callMethod<void>("completeAction");
	}
}

extern "C" {

JNIEXPORT void JNICALL Java_de_skycoder42_remindme_RemindmeService_initIntent(JNIEnv */*env*/, jobject /*obj*/, jstring action, jstring data)
{
	auto jAction = QAndroidJniObject(action).toString();
	auto jData = QAndroidJniObject(data).toString();
	AndroidNotifier::initIntent(jAction, jData);
}

}

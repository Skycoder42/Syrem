#include "androidnotifier.h"
#include "androidscheduler.h"
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <registry.h>

bool AndroidNotifier::_canInvoke = false;
QMutex AndroidNotifier::_invokeMutex;
QList<AndroidNotifier::Intent> AndroidNotifier::_intentCache;
QSet<QUuid> AndroidNotifier::_blockList;

AndroidNotifier::AndroidNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_setup(false),
	_setupIds(),
	_actionIds()
{}

void AndroidNotifier::handleIntent(const QString &action, const QUuid &id, quint32 versionCode)
{
	QMutexLocker _(&_invokeMutex);
	auto obj = Registry::acquireObject(INotifier_iid);

	if(action == QStringLiteral("de.skycoder42.remindme.ActionComplete")) {
		_blockList.insert(id);
		if(obj) {
			QMetaObject::invokeMethod(obj, "removeNotification", Qt::QueuedConnection,
									  Q_ARG(QUuid, id));
		}
	}

	_intentCache.append(Intent{action, id, versionCode});
	if(_canInvoke && obj)
		QMetaObject::invokeMethod(obj, "handleIntentImpl", Qt::QueuedConnection);
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
	if(!_intentCache.isEmpty())
		QMetaObject::invokeMethod(this, "handleIntentImpl", Qt::QueuedConnection);
}

void AndroidNotifier::showNotification(const Reminder &reminder)
{
	if(_blockList.contains(reminder.id()))//only show non-blocked notifications
		return;

	auto service = QtAndroid::androidService();
	service.callMethod<void>("notify", "(Ljava/lang/String;IZLjava/lang/String;)V",
							 QAndroidJniObject::fromString(reminder.id().toString()).object(),
							 (jint)reminder.versionCode(),
							 (jboolean)reminder.isImportant(),
							 QAndroidJniObject::fromString(reminder.description()).object());
	if(_setup)
		_setupIds.insert(reminder.id());
}

void AndroidNotifier::removeNotification(const QUuid &id)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("cancelNotify", "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(id.toString()).object());
	if(_setup)
		_setupIds.remove(id);
}

void AndroidNotifier::showErrorMessage(const QString &error)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("notifyError", "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(error).object());
}

void AndroidNotifier::notificationHandled(const QUuid &id, const QString &errorMsg)
{
	//TODO except for inline snooze
	removeNotification(id);
	if(!errorMsg.isNull())
		showErrorMessage(errorMsg);
	_actionIds.remove(id);
	tryQuit();
}

void AndroidNotifier::handleIntentImpl()
{
	QMutexLocker _(&_invokeMutex);

	_blockList.clear(); //unblock all
	foreach(auto entry, _intentCache) {
		auto action = std::get<0>(entry);
		auto id = std::get<1>(entry);
		auto versionCode = std::get<2>(entry);

		if(action == QStringLiteral("de.skycoder42.remindme.ActionScheduler"))
			AndroidScheduler::triggerSchedule(id, versionCode);
		else if(action == QStringLiteral("de.skycoder42.remindme.ActionComplete")) {
			_actionIds.insert(id);
			emit messageCompleted(id, versionCode);
		}
	}
	_intentCache.clear();
	tryQuit();
}

void AndroidNotifier::tryQuit()
{
	if(_actionIds.isEmpty()) {//TODO sync before quit
		auto service = QtAndroid::androidService();
		service.callMethod<void>("completeAction");
	}
}

extern "C" {

JNIEXPORT void JNICALL Java_de_skycoder42_remindme_RemindmeService_handleIntent(JNIEnv */*env*/, jobject /*obj*/, jstring action, jstring id, jint versionCode)
{
	auto jAction = QAndroidJniObject(action).toString();
	auto jId = QAndroidJniObject(id).toString();
	QUuid uId(jId);
	if(uId.isNull())
		return;

	AndroidNotifier::handleIntent(jAction, uId, (quint32)versionCode);
}

}

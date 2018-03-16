#include "androidnotifier.h"
#include "androidscheduler.h"
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QtMvvmCore/ServiceRegistry>
#include <remindmeapp.h>
#include <dateparser.h>
#include <QtDataSync/SyncManager>

const QString AndroidNotifier::ActionScheduler(QStringLiteral("de.skycoder42.remindme.ActionScheduler"));
const QString AndroidNotifier::ActionComplete(QStringLiteral("de.skycoder42.remindme.ActionComplete"));
const QString AndroidNotifier::ActionDismiss(QStringLiteral("de.skycoder42.remindme.ActionDismiss"));
const QString AndroidNotifier::ActionSnooze(QStringLiteral("de.skycoder42.remindme.ActionSnooze"));

bool AndroidNotifier::_canInvoke = false;
QMutex AndroidNotifier::_invokeMutex;
QList<AndroidNotifier::Intent> AndroidNotifier::_intentCache;
QSet<QUuid> AndroidNotifier::_blockList;

AndroidNotifier::AndroidNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_setup(false),
	_setupIds(),
	_actionIds(),
	_shouldSync(false)
{}

void AndroidNotifier::guiStarted()
{
	QMutexLocker _(&_invokeMutex);

	if(!_intentCache.isEmpty()) {
		auto intent = _intentCache.last();
		_intentCache.clear();
		QMetaObject::invokeMethod(coreApp, "showSnoozeControl", Qt::QueuedConnection,
								  Q_ARG(QUuid, std::get<1>(intent)),
								  Q_ARG(quint32, std::get<2>(intent)));
	}
	_canInvoke = true;
}

void AndroidNotifier::serviceStarted()
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("scheduleAutoCheck");
}

void AndroidNotifier::handleServiceIntent(const QString &action, const QUuid &id, quint32 versionCode, const QString &result)
{
	qDebug() << Q_FUNC_INFO << action << id << versionCode << result;
	QMutexLocker _(&_invokeMutex);
	auto obj = QtMvvm::ServiceRegistry::instance()->serviceObj(INotifier_iid);
	qDebug() << Q_FUNC_INFO << obj;

	//block notifications from beeing re-shown while starting up
	if(action != ActionScheduler) {
		if(id.isNull())
			return;
		_blockList.insert(id);
	} if(action == ActionComplete) {
		if(obj) {
			QMetaObject::invokeMethod(obj, "removeNotification", Qt::QueuedConnection,
									  Q_ARG(QUuid, id));
		}
	}

	_intentCache.append(Intent{action, id, versionCode, result});
	if(_canInvoke && obj)
		QMetaObject::invokeMethod(obj, "handleIntentImpl", Qt::QueuedConnection);
}

void AndroidNotifier::handleActivityIntent(const QString &action, const QUuid &id, quint32 versionCode)
{
	QMutexLocker _(&_invokeMutex);

	if(action == ActionSnooze) {
		if(_canInvoke) {
			QMetaObject::invokeMethod(coreApp, "showSnoozeControl", Qt::QueuedConnection,
									  Q_ARG(QUuid, id),
									  Q_ARG(quint32, versionCode));
		} else
			_intentCache.append(Intent{action, id, versionCode, {}});
	}
}

void AndroidNotifier::beginSetup()
{
	_setupIds.clear();

	auto service = QtAndroid::androidService();
	service.callMethod<void>("ensureCanActive");

	_setup = true;
}

void AndroidNotifier::endSetup()
{
	_setup = false;

	QAndroidJniEnvironment env;
	auto service = QtAndroid::androidService();
	auto keys = service.callObjectMethod("activeNotifications", "()[Ljava/lang/String;");
	if(keys.isValid() && keys.object() == nullptr) {
		auto cnt = env->GetArrayLength(keys.object<jobjectArray>());
		for(auto i = 0; i < cnt; i++) {
			QAndroidJniObject obj(env->GetObjectArrayElement(keys.object<jobjectArray>(), i));
			QUuid keyId(obj.toString());
			if(!keyId.isNull() && !_setupIds.contains(keyId))
				removeNotification(keyId);
		}
	}

	_setupIds.clear();
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
							 QAndroidJniObject::fromString(reminder.id().toString()).object<jstring>(),
							 (jint)reminder.versionCode(),
							 (jboolean)reminder.isImportant(),
							 QAndroidJniObject::fromString(reminder.description()).object<jstring>());
	if(_setup)
		_setupIds.insert(reminder.id());
}

void AndroidNotifier::removeNotification(const QUuid &id)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("cancelNotify", "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(id.toString()).object<jstring>());
	if(_setup)
		_setupIds.remove(id);
}

void AndroidNotifier::showErrorMessage(const QString &error)
{
	auto service = QtAndroid::androidService();
	service.callMethod<void>("notifyError", "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(error).object<jstring>());
}

void AndroidNotifier::notificationHandled(const QUuid &id, const QString &errorMsg)
{
	removeNotification(id);
	if(!errorMsg.isNull())
		showErrorMessage(errorMsg);
	_actionIds.remove(id);
	tryQuit();
}

void AndroidNotifier::handleIntentImpl()
{
	qDebug() << Q_FUNC_INFO << _blockList;
	QMutexLocker _(&_invokeMutex);

	_blockList.clear(); //unblock all
	foreach(auto entry, _intentCache) {
		auto action = std::get<0>(entry);
		auto id = std::get<1>(entry);
		auto versionCode = std::get<2>(entry);
		auto result = std::get<3>(entry);
		qDebug() << Q_FUNC_INFO << action << id << versionCode << result;

		if(action == ActionScheduler) {
			if(!id.isNull())
				AndroidScheduler::triggerSchedule(id, versionCode);
		} else if(action == ActionComplete) {
			_shouldSync = true;
			_actionIds.insert(id);
			emit messageCompleted(id, versionCode);
		} else if(action == ActionDismiss) {
			_shouldSync = true;
			_actionIds.insert(id);
			emit messageDismissed(id, versionCode);
		} else if(action == ActionSnooze) {
			try {
				DateParser parser;
				auto res = parser.snoozeParse(result);
				_shouldSync = true;
				emit messageDelayed(id, versionCode, res);
			} catch (QString &s) {
				auto service = QtAndroid::androidService();
				service.callMethod<void>("notifyReminderError", "(Ljava/lang/String;ILjava/lang/String;)V",
										 QAndroidJniObject::fromString(id.toString()).object<jstring>(),
										 (jint)versionCode,
										 QAndroidJniObject::fromString(s).object<jstring>());
			}
		}
	}
	_intentCache.clear();
	tryQuit();
}

void AndroidNotifier::tryQuit()
{
	qDebug() << Q_FUNC_INFO << _actionIds << _shouldSync;
	if(_actionIds.isEmpty()) {
		if(_shouldSync) {
			qDebug() << Q_FUNC_INFO << "sync";
			_shouldSync = false;
			auto manager = new QtDataSync::SyncManager(this);
			manager->runOnSynchronized([](QtDataSync::SyncManager::SyncState state) {
				qDebug() << Q_FUNC_INFO << state;
				auto service = QtAndroid::androidService();
				service.callMethod<void>("completeAction");
			});
		} else {
			qDebug() << Q_FUNC_INFO << "no sync";
			auto service = QtAndroid::androidService();
			service.callMethod<void>("completeAction");
		}
	}
}

extern "C" {

JNIEXPORT void JNICALL Java_de_skycoder42_remindme_RemindmeService_handleIntent(JNIEnv */*env*/, jobject /*obj*/, jstring action, jstring id, jint versionCode, jstring resultExtra)
{
	auto jAction = QAndroidJniObject(action).toString();
	auto jId = QAndroidJniObject(id).toString();
	auto jResult = QAndroidJniObject(resultExtra).toString();
	QUuid uId(jId);

	AndroidNotifier::handleServiceIntent(jAction, uId, (quint32)versionCode, jResult);
}

JNIEXPORT void JNICALL Java_de_skycoder42_remindme_RemindmeActivity_handleIntent(JNIEnv */*env*/, jobject /*obj*/, jstring action, jstring id, jint versionCode)
{
	auto jAction = QAndroidJniObject(action).toString();
	auto jId = QAndroidJniObject(id).toString();
	QUuid uId(jId);

	AndroidNotifier::handleActivityIntent(jAction, uId, (quint32)versionCode);
}

}

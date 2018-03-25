#include "remindmeservice.h"
#include <QtAndroid>

const QString RemindmeService::ActionScheduler { QStringLiteral("de.skycoder42.remindme.Action.Scheduler") };

QMutex RemindmeService::_runMutex;
QPointer<RemindmeService> RemindmeService::_runInstance = nullptr;
QList<RemindmeService::Intent> RemindmeService::_currentIntents;

RemindmeService::RemindmeService(QObject *parent) :
	QObject(parent),
	_store(nullptr),
	_manager(nullptr),
	_scheduler(new AndroidScheduler(this)),
	_notifier(new AndroidNotifier(this))
{}

bool RemindmeService::startService()
{
	//load translations
	RemindMe::prepareTranslations(QStringLiteral("remindmed"));

	try {
		QtDataSync::Setup setup;
		RemindMe::setup(setup);
		setup.create();

		_store = new ReminderStore(this);
		connect(_store, &QtDataSync::DataTypeStoreBase::dataChanged,
				this, &RemindmeService::dataChanged);

		_manager = new QtDataSync::SyncManager(this);
		_manager->runOnSynchronized([this](QtDataSync::SyncManager::SyncState state){
			qDebug() << "Synchronization completed in state" << state;
			handleAllIntents();
		});

		qInfo() << "service successfully started";
		return true;
	} catch(QException &e) {
		qCritical() << e.what();
		return false;
	}
}

void RemindmeService::handleIntent(const Intent &intent)
{
	QMutexLocker locker(&_runMutex);
	_currentIntents.append(intent);
	if(_runInstance) {
		QMetaObject::invokeMethod(_runInstance, "handleAllIntents", Qt::QueuedConnection);
		_runInstance = nullptr;//set to null again, this way only 1 queued invokation is done
	}
}

void RemindmeService::dataChanged(const QString &key, const QVariant &value)
{
	if(value.isValid()) {
		auto reminder = value.value<Reminder>();
		if(!_scheduler->scheduleReminder(reminder))
			_notifier->showNotification(reminder);
	} else {
		QUuid id(key);
		_scheduler->cancleReminder(id);
	}
}

void RemindmeService::handleAllIntents()
{
	QMutexLocker locker(&_runMutex);
	//Set self as instance to accept further intents
	_runInstance = this;

	if(_currentIntents.isEmpty())
		return;

	for(auto intent : _currentIntents) {
		if(intent.action == ActionScheduler)
			actionSchedule(intent.reminderId, intent.versionCode);
		else
			qWarning() << "Received unknown intent action:" << intent.action;
	}
	_currentIntents.clear();

	QMetaObject::invokeMethod(this, "tryQuit", Qt::QueuedConnection);
}

void RemindmeService::tryQuit()
{
	_manager->runOnSynchronized([this](QtDataSync::SyncManager::SyncState state){
		//if new intents have been added - postpone the quitting
		QMutexLocker locker(&_runMutex);
		if(!_currentIntents.isEmpty())
			return;

		qDebug() << "Synchronization result right before stopping service:" << state;
		auto service = QtAndroid::androidService();
		service.callMethod<void>("completeAction");
	});
}

void RemindmeService::actionSchedule(const QUuid &id, quint32 versionCode)
{
	try {
		auto reminder = _store->load(id);
		if(reminder.versionCode() != versionCode) {
			qInfo() << "Skipping notification of changed reminder" << id;
			return;
		}

		_notifier->showNotification(reminder);
	} catch(QtDataSync::NoDataException &e) {
		Q_UNUSED(e)
		qInfo() << "Skipping notification of deleted reminder" << id;
	} catch(QException &e) {
		qCritical() << "Failed to load reminder with id" << id
					<< "to show a notification with error:" << e.what();
		_notifier->showErrorMessage(tr("Failed to load details of triggered reminder!"));
	}
}

extern "C" {

JNIEXPORT void JNICALL Java_de_skycoder42_remindme_RemindmeService_handleIntent(JNIEnv */*env*/, jobject /*obj*/, jstring action, jstring id, jint versionCode, jstring resultExtra)
{
	RemindmeService::handleIntent({
									  QAndroidJniObject(action).toString(),
									  QUuid(QAndroidJniObject(id).toString()),
									  (quint32)versionCode,
									  QAndroidJniObject(resultExtra).toString()
								  });
}

}

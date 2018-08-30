#include "remindmeservice.h"
#include <QtAndroid>
#include <QRemoteObjectReplica>
#include <QtMvvmCore/ServiceRegistry>
#include <localsettings.h>

const QString RemindmeService::ActionScheduler { QStringLiteral("de.skycoder42.remindme.Action.Scheduler") };
const QString RemindmeService::ActionComplete { QStringLiteral("de.skycoder42.remindme.Action.Complete") };
const QString RemindmeService::ActionSnooze { QStringLiteral("de.skycoder42.remindme.Action.Snooze") };
const QString RemindmeService::ActionRefresh { QStringLiteral("de.skycoder42.remindme.Action.Refresh") };
const QString RemindmeService::ActionSetup { QStringLiteral("de.skycoder42.remindme.Action.Setup") };

QMutex RemindmeService::_runMutex;
QPointer<RemindmeService> RemindmeService::_runInstance = nullptr;
QList<RemindmeService::Intent> RemindmeService::_currentIntents;

RemindmeService::RemindmeService(QObject *parent) :
	QObject(parent),
	_store(nullptr),
	_manager(nullptr),
	_parser(nullptr),
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

		_parser = QtMvvm::ServiceRegistry::instance()->service<EventExpressionParser>();

		_store = new ReminderStore(this);
		connect(_store, &QtDataSync::DataTypeStoreBase::dataChanged,
				this, &RemindmeService::dataChanged);
		connect(_store, &QtDataSync::DataTypeStoreBase::dataResetted,
				this, &RemindmeService::dataResetted);

		_manager = new QtDataSync::SyncManager(this);
		auto runFn = [this](){
			_manager->runOnSynchronized([this](QtDataSync::SyncManager::SyncState state){
				qDebug() << "Synchronization completed in state" << state;
				handleAllIntents();
			});
		};

		if(_manager->replica()->isInitialized())
			runFn();
		else {
			connect(_manager->replica(), &QRemoteObjectReplica::initialized,
					this, runFn);
		}

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

void RemindmeService::dataResetted()
{
	// Android alarms cannot be easily canceled -> just let them be, they will do nothing as the corresponding reminders are deleted
	_notifier->cleanNotifications();
}

void RemindmeService::dataChanged(const QString &key, const QVariant &value)
{
	if(value.isValid()) {
		auto reminder = value.value<Reminder>();
		doSchedule(reminder);
	} else {
		QUuid id(key);
		_scheduler->cancleReminder(id);
		_notifier->removeNotification(id);
		removeNotify(id);
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
		else if(intent.action == ActionComplete)
			actionComplete(intent.reminderId, intent.versionCode);
		else if(intent.action == ActionSnooze)
			actionSnooze(intent.reminderId, intent.versionCode, intent.result);
		else if(intent.action == ActionRefresh)
			qDebug() << "Completed datasync synchronization";
		else if(intent.action == ActionSetup)
			actionSetup();
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

		LocalSettings::instance()->accessor()->sync();

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
		addNotify(id);
	} catch(QtDataSync::NoDataException &e) {
		Q_UNUSED(e)
		qInfo() << "Skipping notification of deleted reminder" << id;
	} catch(QException &e) {
		qCritical() << "Failed to load reminder with id" << id
					<< "to show a notification with error:" << e.what();
		_notifier->showErrorMessage(tr("Failed to load details of triggered reminder!"));
	}
}

void RemindmeService::actionComplete(const QUuid &id, quint32 versionCode)
{
	try {
		auto reminder = _store->load(id);
		if(reminder.versionCode() != versionCode) {
			qInfo() << "Skipping completing of changed reminder" << id;
			return;
		}
		reminder.nextSchedule(_store->store(), QDateTime::currentDateTime());
		// TODO
//		if(_settings->scheduler.urlOpen)
//			rem.openUrls();
	} catch(QtDataSync::NoDataException &e) {
		Q_UNUSED(e)
		qInfo() << "Skipping completing of deleted reminder" << id;
	} catch(QException &e) {
		qCritical() << "Failed to load reminder with id" << id
					<< "to complete it with error:" << e.what();
	}
}

void RemindmeService::actionSnooze(const QUuid &id, quint32 versionCode, const QString &expression)
{
	try {
		auto reminder = _store->load(id);
		try {
			auto term = _parser->parseExpression(expression);
			if(_parser->needsSelection(term)) {
				throw EventExpressionParserException{tr("Entered expression has multiple interpretations. "
														"Use the app to handle this or enter a unique expression")};
			}
			auto snooze = _parser->evaluteTerm(term.first());

			if(reminder.versionCode() != versionCode) {
				qInfo() << "Skipping completing of changed reminder" << id;
				return;
			}

			reminder.performSnooze(_store->store(), snooze);
		} catch (EventExpressionParserException &e) {
			_notifier->showParserError(reminder, e.message());
		}
	} catch(QtDataSync::NoDataException &e) {
		Q_UNUSED(e)
		qInfo() << "Skipping completing of deleted reminder" << id;
	} catch(QException &e) {
		qCritical() << "Failed to load reminder with id" << id
					<< "to complete it with error:" << e.what();
	}
}

void RemindmeService::actionSetup()
{
	try {
		LocalSettings::instance()->service.badgeActive.reset();
		auto reminders = _store->loadAll();
		for(auto reminder : reminders)
			doSchedule(reminder);
	} catch(QException &e) {
		qCritical() << "Failed to initially load reminders for setup with error:" << e.what();
	}
}

void RemindmeService::doSchedule(const Reminder &reminder)
{
	_notifier->removeNotification(reminder.id());
	if(!_scheduler->scheduleReminder(reminder)) {
		_notifier->showNotification(reminder);
		addNotify(reminder.id());
	}
}

void RemindmeService::addNotify(const QUuid &id)
{
	QSet<QUuid> active = LocalSettings::instance()->service.badgeActive;
	qDebug() << active;
	if(!active.contains(id)) {
		active.insert(id);
		LocalSettings::instance()->service.badgeActive = active;
		updateNotificationCount(active.size());
	}
}

void RemindmeService::removeNotify(const QUuid &id)
{
	QSet<QUuid> active = LocalSettings::instance()->service.badgeActive;
	qDebug() << active;
	if(active.remove(id)) {
		LocalSettings::instance()->service.badgeActive = active;
		updateNotificationCount(active.size());
	}
}

void RemindmeService::updateNotificationCount(int count)
{
	qDebug() << "Updating badge count to" << count;
	QAndroidJniObject::callStaticMethod<jboolean>("me/leolin/shortcutbadger/ShortcutBadger", "applyCount",
												  "(Landroid/content/Context;I)Z",
												  QtAndroid::androidContext().object(),
												  (jint)count);
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

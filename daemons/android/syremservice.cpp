#include "syremservice.h"
#include <QtAndroid>
#include <QAndroidJniExceptionCleaner>
#include <QRemoteObjectReplica>
#include <QtMvvmCore/ServiceRegistry>
#include <localsettings.h>

const QString SyremService::ActionScheduler { QStringLiteral("de.skycoder42.syrem.Action.Scheduler") };
const QString SyremService::ActionComplete { QStringLiteral("de.skycoder42.syrem.Action.Complete") };
const QString SyremService::ActionSnooze { QStringLiteral("de.skycoder42.syrem.Action.Snooze") };
const QString SyremService::ActionRefresh { QStringLiteral("de.skycoder42.syrem.Action.Refresh") };
const QString SyremService::ActionSetup { QStringLiteral("de.skycoder42.syrem.Action.Setup") };

SyremService::SyremService(int &argc, char **argv) :
	Service{argc, argv}
{
	QCoreApplication::setApplicationName(QStringLiteral(PROJECT_TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE));

	qRegisterMetaTypeStreamOperators<QSet<QUuid>>();

	addCallback("onStartCommand", &SyremService::onStartCommand);
}

void SyremService::dataResetted()
{
	// Android alarms cannot be easily canceled -> just let them be, they will do nothing as the corresponding reminders are deleted
	_notifier->cleanNotifications();
	LocalSettings::instance()->service.badgeActive.reset();
	updateNotificationCount(0);
}

void SyremService::dataChanged(const QString &key, const QVariant &value)
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

void SyremService::handleAllIntents()
{
	QMutexLocker locker{&_runMutex};
	if(_currentIntents.isEmpty())
		return;
	const auto intents = std::move(_currentIntents);
	_currentIntents = {};
	locker.unlock();

	QList<int> startIds;
	for(const auto &intent : intents) {
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
		startIds.append(intent.startId);
	}
	_doneStartIds.enqueue(startIds);

	_manager->runOnSynchronized([this](QtDataSync::SyncManager::SyncState state){
		//if new intents have been added - postpone the quitting
		LocalSettings::instance()->accessor()->sync();
		qDebug() << "Synchronization result right before completing" << _doneStartIds.size() << "service intents:" << state;
		auto service = QtAndroid::androidService();
		Q_ASSERT(!_doneStartIds.isEmpty());
		for(const auto &startId : _doneStartIds.dequeue()) {
			service.callMethod<void>("completeAction", "(I)V",
									 static_cast<jint>(startId));
		}
	}, true);
}

void SyremService::actionSchedule(const QUuid &id, quint32 versionCode)
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

void SyremService::actionComplete(const QUuid &id, quint32 versionCode)
{
	try {
		auto reminder = _store->load(id);
		if(reminder.versionCode() != versionCode) {
			qInfo() << "Skipping completing of changed reminder" << id;
			return;
		}
		reminder.nextSchedule(_store->store(), QDateTime::currentDateTime());
		if(_settings->scheduler.urlOpen)
			reminder.openUrls(); //TODO use android native stuff
	} catch(QtDataSync::NoDataException &e) {
		Q_UNUSED(e)
		qInfo() << "Skipping completing of deleted reminder" << id;
	} catch(QException &e) {
		qCritical() << "Failed to load reminder with id" << id
					<< "to complete it with error:" << e.what();
	}
}

void SyremService::actionSnooze(const QUuid &id, quint32 versionCode, const QString &expression)
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

void SyremService::actionSetup()
{
	try {
		LocalSettings::instance()->service.badgeActive.reset();
		auto reminders = _store->loadAll();
		for(const auto &reminder : reminders)
			doSchedule(reminder);
	} catch(QException &e) {
		qCritical() << "Failed to initially load reminders for setup with error:" << e.what();
	}
}

QtService::Service::CommandMode SyremService::onStart()
{
	//load translations
	Syrem::prepareTranslations(QStringLiteral("syremd"));

	try {
		QtDataSync::Setup setup;
		Syrem::setup(setup);
		setup.create();

		QtMvvm::ServiceRegistry::instance()->injectServices(this);
		_scheduler = new AndroidScheduler{this};
		_notifier = new AndroidNotifier{this};

		_store = new ReminderStore{this};
		connect(_store, &QtDataSync::DataTypeStoreBase::dataChanged,
				this, &SyremService::dataChanged);
		connect(_store, &QtDataSync::DataTypeStoreBase::dataResetted,
				this, &SyremService::dataResetted);

		_manager = new QtDataSync::SyncManager{this};
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
		return Synchronous;
	} catch(QException &e) {
		qCritical() << e.what();
		qApp->exit(EXIT_FAILURE); //TODO correct way
		return Synchronous;
	}
}

void SyremService::doSchedule(const Reminder &reminder)
{
	_notifier->removeNotification(reminder.id());
	if(!_scheduler->scheduleReminder(reminder)) {
		_notifier->showNotification(reminder);
		addNotify(reminder.id());
	}
}

void SyremService::addNotify(const QUuid &id)
{
	QSet<QUuid> active = LocalSettings::instance()->service.badgeActive;
	qDebug() << active;
	if(!active.contains(id)) {
		active.insert(id);
		LocalSettings::instance()->service.badgeActive = active;
		updateNotificationCount(active.size());
	}
}

void SyremService::removeNotify(const QUuid &id)
{
	QSet<QUuid> active = LocalSettings::instance()->service.badgeActive;
	qDebug() << active;
	if(active.remove(id)) {
		LocalSettings::instance()->service.badgeActive = active;
		updateNotificationCount(active.size());
	}
}

void SyremService::updateNotificationCount(int count)
{
	qDebug() << "Updating badge count to" << count;
	QAndroidJniObject::callStaticMethod<jboolean>("me/leolin/shortcutbadger/ShortcutBadger", "applyCount",
												  "(Landroid/content/Context;I)Z",
												  QtAndroid::androidContext().object(),
												  static_cast<jint>(count));
}

int SyremService::onStartCommand(const QAndroidIntent &intent, int flags, int startId)
{
	Q_UNUSED(flags)

	if(intent.handle().isValid()) {
		QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
		auto action = intent.handle().callObjectMethod("getAction", "()Ljava/lang/String;").toString();
		auto remId = intent.handle().callObjectMethod("getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;",
													   QAndroidJniObject::fromString(QStringLiteral("id")).object()).toString();
		auto versionCode = intent.handle().callMethod<jint>("getIntExtra", "(Ljava/lang/String;I)I",
															QAndroidJniObject::fromString(QStringLiteral("versionCode")).object());
		auto resExtra = QtAndroid::androidService().callObjectMethod("handleIntent", "(Landroid/content/Intent;)Ljava/lang/String;",
																   intent.handle().object());

		QMutexLocker locker{&_runMutex};
		_currentIntents.append({
								   action,
								   remId,
								   static_cast<quint32>(versionCode),
								   resExtra.isValid() ? resExtra.toString() : QString{},
								   startId
							   });
		QMetaObject::invokeMethod(this, "handleAllIntents", Qt::QueuedConnection);
	}

	return 0x00000003; //START_REDELIVER_INTENT
}
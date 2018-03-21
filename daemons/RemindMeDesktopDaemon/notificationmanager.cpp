#include "notificationmanager.h"
#include <QRemoteObjectReplica>
#include <QTimer>
#include <QDebug>
#include <QLoggingCategory>
#include <chrono>
using namespace QtDataSync;

Q_LOGGING_CATEGORY(manager, "manager")
Q_LOGGING_CATEGORY(notifier, "notifier")

NotificationManager::NotificationManager(QObject *parent) :
	QObject(parent),
	_scheduler(new TimerScheduler(this)),
	_notifier(nullptr),
	_settings(nullptr),
	_manager(new SyncManager(this)),
	_store(new DataTypeStore<Reminder, QUuid>(this))
{}

void NotificationManager::init()
{
	connect(_scheduler, &TimerScheduler::scheduleTriggered,
			this, &NotificationManager::scheduleTriggered);

	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageCompleted(QUuid,quint32)),
			this, SLOT(messageCompleted(QUuid,quint32)));
	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageDelayed(QUuid,quint32,QDateTime)),
			this, SLOT(messageDelayed(QUuid,quint32,QDateTime)));
	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageActivated(QUuid)),
			this, SLOT(messageActivated(QUuid)));

	auto runFn = [this](){
		_manager->runOnSynchronized([this](SyncManager::SyncState state) {
			Q_UNUSED(state)
			try {
				_scheduler->initialize(_store->loadAll());

				connect(_store, &DataTypeStoreBase::dataChanged,
						this, &NotificationManager::dataChanged);
			} catch(QException &e) {
				qCCritical(manager) << "Failed to load stored reminders with error:" << e.what();
				_notifier->showErrorMessage(tr("Failed to load any reminders!"));
			}
		});
	};

	if(_manager->replica()->isInitialized())
		runFn();
	else {
		connect(_manager->replica(), &QRemoteObjectReplica::initialized,
				this, runFn);
	}
}

void NotificationManager::scheduleTriggered(const QUuid &id)
{
	try {
		auto rem = _store->load(id);
		_notifier->showNotification(rem);
	} catch(QException &e) {
		qCCritical(manager) << "Failed to load reminder with id" << id
							<< "to display notification with error:" << e.what();
		//show error message here, because triggering a reminder failed -> get attention
		_notifier->showErrorMessage(tr("Failed to load reminder to display notification!"));
	}
}

void NotificationManager::messageCompleted(const QUuid &id, quint32 versionCode)
{
	try {
		auto rem = _store->load(id);
		if(rem.versionCode() == versionCode) {
			rem.nextSchedule(_store->store(), QDateTime::currentDateTime());
			qCInfo(manager) << "Completed reminder with id" << id;
		}
	} catch(QException &e) {
		qCCritical(manager) << "Failed to complete reminder with id" << id
							<< "with error:" << e.what();
	}
}

void NotificationManager::messageDelayed(const QUuid &id, quint32 versionCode, QDateTime nextTrigger)
{
	if(!nextTrigger.isValid())
		return;

	if(QDateTime::currentDateTime().secsTo(nextTrigger) < 60)
		nextTrigger = QDateTime::currentDateTime().addSecs(60);

	try {
		auto rem = _store->load(id);
		if(rem.versionCode() == versionCode) {
			rem.performSnooze(_store->store(), nextTrigger);
			qCInfo(manager) << "Snoozed reminder with id" << id;
		}
	} catch(QException &e) {
		qCCritical(manager) << "Failed to snooze reminder with id" << id
							<< "with error:" << e.what();
	}
}

void NotificationManager::messageActivated(const QUuid &id)
{
	//TODO show gui app with id
	Q_UNIMPLEMENTED();
}

void NotificationManager::dataChanged(const QString &key, const QVariant &value)
{
	if(value.isValid()) {
		auto reminder = value.value<Reminder>();
		_notifier->removeNotification(reminder.id());
		_scheduler->scheduleReminder(reminder);
	} else {
		_scheduler->cancleReminder(QUuid(key));
		_notifier->removeNotification(QUuid(key));
	}
}

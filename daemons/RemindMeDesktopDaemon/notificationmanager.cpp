#include "notificationmanager.h"
#include <QRemoteObjectReplica>
#include <QTimer>
#include <QDebug>
#include <QLoggingCategory>
#include <QProcess>
#include <QStandardPaths>
#include <chrono>
#include <QCoreApplication>
using namespace QtDataSync;

Q_LOGGING_CATEGORY(manager, "manager")
Q_LOGGING_CATEGORY(notifier, "notifier")

NotificationManager::NotificationManager(QObject *parent) :
	QObject(parent),
	_scheduler(new TimerScheduler(this)),
	_taskbar(new QTaskbarControl(new QWidget())),
	_notifier(nullptr),
	_settings(nullptr),
	_manager(new SyncManager(this)),
	_store(new DataTypeStore<Reminder, QUuid>(this)),
	_activeIds()
{
	connect(this, &NotificationManager::destroyed,
			_taskbar->parent(), &QObject::deleteLater);
}

void NotificationManager::init()
{
	_taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, QStringLiteral("remind-me.desktop"));

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
				connect(_store, &DataTypeStoreBase::dataResetted,
						this, &NotificationManager::dataResetted);
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

void NotificationManager::triggerSync()
{
	qCInfo(manager) << "Reconnecting to remote server";
	_manager->reconnect();
}

void NotificationManager::scheduleTriggered(const QUuid &id)
{
	try {
		auto rem = _store->load(id);
		_notifier->showNotification(rem);
		addNotify(id);
	} catch(QtDataSync::NoDataException &e) {
		qCDebug(manager) << "Skipping presentation of deleted reminder" << id
						<< "with reason" << e.what();
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
		removeNotify(id);
	} catch(QtDataSync::NoDataException &e) {
		qCDebug(manager) << "Skipping completing of deleted reminder" << id
						<< "with reason" << e.what();
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
		removeNotify(id);
	} catch(QtDataSync::NoDataException &e) {
		qCDebug(manager) << "Skipping snoozing of deleted reminder" << id
						<< "with reason" << e.what();
	} catch(QException &e) {
		qCCritical(manager) << "Failed to snooze reminder with id" << id
							<< "with error:" << e.what();
	}
}

void NotificationManager::messageActivated(const QUuid &id)
{
	auto program = QStandardPaths::findExecutable(QStringLiteral("remind-me"));
	if(program.isEmpty()) {
		qCWarning(manager) << "Failed to find the remind-me executable!";
		return;
	}
	QStringList args {
		QStringLiteral("--select"),
		id.toString()
	};
	if(QProcess::startDetached(program, args))
		qCDebug(manager) << "Successfully launched" << program;
	else
		qCWarning(manager) << "Failed to launch" << program;
}

void NotificationManager::dataChanged(const QString &key, const QVariant &value)
{
	if(value.isValid()) {
		auto reminder = value.value<Reminder>();
		_notifier->removeNotification(reminder.id());
		removeNotify(reminder.id());
		_scheduler->scheduleReminder(reminder);
	} else {
		QUuid id(key);
		_scheduler->cancleReminder(id);
		_notifier->removeNotification(id);
		removeNotify(id);
	}
}

void NotificationManager::dataResetted()
{
	_scheduler->cancelAll();
	_notifier->cancelAll();
}

void NotificationManager::addNotify(const QUuid &id)
{
	if(!_activeIds.contains(id)) {
		_activeIds.insert(id);
		updateNotificationCount();
	}
}

void NotificationManager::removeNotify(const QUuid &id)
{
	if(_activeIds.remove(id))
		updateNotificationCount();
}

void NotificationManager::updateNotificationCount()
{
	auto nValue = _activeIds.count();
	if(nValue <= 0) {
		if(_taskbar->counterVisible()) {
			_taskbar->setCounter(0);
			_taskbar->setCounterVisible(false);
		}
	} else {
		_taskbar->setCounter(nValue);
		if(!_taskbar->counterVisible())
			_taskbar->setCounterVisible(true);
	}
}

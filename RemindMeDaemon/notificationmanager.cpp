#include "notificationmanager.h"
#include "registry.h"
#include <QTimer>
#include <QDebug>
#include <chrono>
using namespace QtDataSync;

NotificationManager::NotificationManager(QObject *parent) :
	QObject(parent),
	_scheduler(Registry::acquire<IScheduler>()),
	_notifier(Registry::acquire<INotifier>()),
	_settings(new QSettings(this)),
	_manager(new SyncManager(this)),
	_store(new DataTypeStore<Reminder, QUuid>(this))
{
	Q_ASSERT(_scheduler);
	Q_ASSERT(_notifier);

	_settings->beginGroup(QStringLiteral("daemon"));

	connect(dynamic_cast<QObject*>(_scheduler), SIGNAL(scheduleTriggered(QUuid)),
			this, SLOT(scheduleTriggered(QUuid)));

	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageDismissed(QUuid,quint32)),
			this, SLOT(messageDismissed(QUuid,quint32)));
	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageCompleted(QUuid,quint32)),
			this, SLOT(messageCompleted(QUuid,quint32)));
	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageDelayed(QUuid,quint32,QDateTime)),
			this, SLOT(messageDelayed(QUuid,quint32,QDateTime)));

	_manager->runOnDownloaded([this](SyncManager::SyncState state) {
		Q_UNUSED(state)
		try {
			auto reminders = _store->loadAll();
			_notifier->beginSetup();
			_scheduler->initialize(reminders);
			_notifier->endSetup();

			connect(_store, &DataTypeStoreBase::dataChanged,
					this, &NotificationManager::dataChanged);
		} catch(QException &e) {
			qCritical() << "Failed to load stored reminders with error:" << e.what();
			_notifier->showErrorMessage(tr("Failed to load any reminders!"));
		}
	});

}

void NotificationManager::scheduleTriggered(const QUuid &id)
{
	try {
		auto rem = _store->load(id);
		_notifier->showNotification(rem);
	} catch(QException &e) {
		qCritical() << "Failed to load reminder to display notification with error:" << e.what();
		_notifier->showErrorMessage(tr("Failed to load reminder to display notification!"));
	}
}

void NotificationManager::messageDismissed(const QUuid &id, quint32 versionCode)
{
	using namespace std::chrono;

	auto defaultSnooze = _settings->value(QStringLiteral("snooze"), 0).toInt();
	if(defaultSnooze < 1)
		defaultSnooze = 20;//default is 20 minutes
	auto snoozeTime = QDateTime::currentDateTime().addSecs(duration_cast<seconds>(minutes(defaultSnooze)).count());

	try {
		auto rem = _store->load(id);
		if(rem.versionCode() == versionCode)
			rem.performSnooze(_store, snoozeTime);
		_notifier->notificationHandled(id);
	} catch(QException &e) {
		qCritical() << "Failed to snooze reminder with error:" << e.what();
		_notifier->notificationHandled(id, tr("Failed to snooze reminder!"));
	}
}

void NotificationManager::messageCompleted(const QUuid &id, quint32 versionCode)
{
	try {
		auto rem = _store->load(id);
		if(rem.versionCode() == versionCode)
			rem.nextSchedule(_store, QDateTime::currentDateTime());
		_notifier->notificationHandled(id);
	} catch(QException &e) {
		qCritical() << "Failed to complete reminder with error:" << e.what();
		_notifier->notificationHandled(id, tr("Failed to complete reminder!"));
	}
}

void NotificationManager::messageDelayed(const QUuid &id, quint32 versionCode, QDateTime nextTrigger)
{
	if(!nextTrigger.isValid())
		messageDismissed(id, versionCode);
	else {
		try {
			if(QDateTime::currentDateTime().secsTo(nextTrigger) < 60)
				nextTrigger = QDateTime::currentDateTime().addSecs(60);

			auto rem = _store->load(id);
			if(rem.versionCode() == versionCode)
				rem.performSnooze(_store, nextTrigger);
			_notifier->notificationHandled(id);
		} catch(QException &e) {
			qCritical() << "Failed to snooze reminder with error:" << e.what();
			_notifier->notificationHandled(id, tr("Failed to snooze reminder!"));
		}
	}
}

void NotificationManager::dataChanged(const QString &key, const QVariant &value)
{
	if(value.isValid()) {
		auto reminder = value.value<Reminder>();
		_notifier->removeNotification(reminder.id());
		if(!_scheduler->scheduleReminder(reminder)) {
			qCritical() << "Failed to schedule reminder";
			_notifier->showErrorMessage(tr("Failed to schedule updated or added reminder!"));
		}
	} else {
		_scheduler->cancleReminder(QUuid(key));
		_notifier->removeNotification(QUuid(key));
	}
}

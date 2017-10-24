#include "notificationmanager.h"
#include "registry.h"
#include <chrono>
using namespace QtDataSync;

NotificationManager::NotificationManager(QObject *parent) :
	QObject(parent),
	_scheduler(Registry::acquire<IScheduler>()),
	_notifier(Registry::acquire<INotifier>()),
	_settings(new QSettings(this)),
	_store(new AsyncDataStore(this))
{
	Q_ASSERT(_scheduler);
	Q_ASSERT(_notifier);

	_settings->beginGroup(QStringLiteral("daemon"));

	connect(dynamic_cast<QObject*>(_scheduler), SIGNAL(scheduleTriggered(QUuid)),
			this, SLOT(scheduleTriggered(QUuid)));

	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageDismissed(Reminder)),
			this, SLOT(messageDismissed(Reminder)));
	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageCompleted(Reminder)),
			this, SLOT(messageCompleted(Reminder)));
	connect(dynamic_cast<QObject*>(_notifier), SIGNAL(messageDelayed(Reminder,QDateTime)),
			this, SLOT(messageDelayed(Reminder,QDateTime)));

	connect(_store, &AsyncDataStore::dataChanged,
			this, &NotificationManager::dataChanged);

	_store->loadAll<Reminder>().onResult([this](QList<Reminder> reminders) {
		if(reminders.isEmpty())
			_notifier->setupEmtpy();
		foreach(auto rem, reminders)
			doSchedule(rem);
	}, [this](const QException &e) {
		qCritical() << "Failed to load stored reminders with error:" << e.what();
		_notifier->showErrorMessage(tr("Failed to load any reminders!"));
	});
}

void NotificationManager::scheduleTriggered(const QUuid &id)
{
	_store->load<Reminder>(id).onResult(this, [this](Reminder rem) {
		_notifier->showNotification(rem);
	}, [this](const QException &e) {
		qCritical() << "Failed to load reminder to display notification with error:" << e.what();
		_notifier->showErrorMessage(tr("Failed to load reminder to display notification!"));
	});
}

void NotificationManager::messageDismissed(Reminder reminder)
{
	using namespace std::chrono;

	auto defaultSnooze = _settings->value(QStringLiteral("snooze/default"), 0).toInt();
	if(defaultSnooze < 1)
		defaultSnooze = 20;//default is 20 minutes
	reminder.setSnooze(QDateTime::currentDateTime().addSecs(duration_cast<seconds>(minutes(defaultSnooze)).count()));

	_store->save(reminder).onResult(this, [](){}, [this](const QException &e) {
		qCritical() << "Failed to save snooze time with error:" << e.what();
		_notifier->showErrorMessage(tr("Failed save snooze time!"));
	});
}

void NotificationManager::messageCompleted(Reminder reminder)
{
	reminder.nextSchedule(_store, QDateTime::currentDateTime()).onResult(this, [](){}, [this](const QException &e) {
		qCritical() << "Failed to complete reminder with error:" << e.what();
		_notifier->showErrorMessage(tr("Failed to complete reminder!"));
	});
}

void NotificationManager::messageDelayed(Reminder reminder, const QDateTime &nextTrigger)
{
	if(!nextTrigger.isValid() || QDateTime::currentDateTime().secsTo(nextTrigger) < 60) //1 minute -> simply use the default stuff
		messageDismissed(reminder);
	else {
		reminder.setSnooze(nextTrigger);
		_store->save(reminder).onResult(this, [](){}, [this](const QException &e) {
			qCritical() << "Failed to save snooze time with error:" << e.what();
			_notifier->showErrorMessage(tr("Failed save snooze time!"));
		});
	}
}

void NotificationManager::dataChanged(int metaTypeId, const QString &key, bool wasDeleted)
{
	if(metaTypeId == qMetaTypeId<Reminder>()) {
		if(wasDeleted) {
			_scheduler->cancleReminder(QUuid(key));
			_notifier->removeNotification(QUuid(key));
		} else {
			_store->load<Reminder>(key).onResult(this, [this](Reminder rem) {
				doSchedule(rem);
			}, [this](const QException &e) {
				qCritical() << "Failed to load reminder with error:" << e.what();
				_notifier->showErrorMessage(tr("Failed to load newly added reminder!"));
			});
		}
	}
}

void NotificationManager::doSchedule(const Reminder &reminder)
{
	_notifier->removeNotification(reminder.id());
	if(reminder.snooze().isValid())
		_scheduler->scheduleReminder(reminder.id(), reminder.snooze());
	else
		_scheduler->scheduleReminder(reminder.id(), reminder.current());
}

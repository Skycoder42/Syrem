#include "notificationmanager.h"
#include "registry.h"
using namespace QtDataSync;

NotificationManager::NotificationManager(QObject *parent) :
	QObject(parent),
	_scheduler(Registry::acquire<IScheduler>()),
	_notifier(Registry::acquire<INotifier>()),
	_store(new AsyncDataStore(this))
{
	Q_ASSERT(_scheduler);
	Q_ASSERT(_notifier);

	connect(dynamic_cast<QObject*>(_scheduler), SIGNAL(scheduleTriggered(QUuid)),
			this, SLOT(scheduleTriggered(QUuid)));

	connect(_store, &AsyncDataStore::dataChanged,
			this, &NotificationManager::dataChanged);

	_store->loadAll<Reminder>().onResult([this](QList<Reminder> reminders) {
		if(reminders.isEmpty())
			_notifier->setupEmtpy();
		foreach(auto rem, reminders)
			_scheduler->scheduleReminder(rem.id(), rem.current());
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

void NotificationManager::dataChanged(int metaTypeId, const QString &key, bool wasDeleted)
{
	if(metaTypeId == qMetaTypeId<Reminder>()) {
		if(wasDeleted) {
			_scheduler->cancleReminder(QUuid(key));
			_notifier->removeNotification(QUuid(key));
		} else {
			_store->load<Reminder>(key).onResult(this, [this](Reminder rem) {
				_scheduler->scheduleReminder(rem.id(), rem.current());
			}, [this](const QException &e) {
				qCritical() << "Failed to load reminder with error:" << e.what();
				_notifier->showErrorMessage(tr("Failed to load newly added reminder!"));
			});
		}
	}
}

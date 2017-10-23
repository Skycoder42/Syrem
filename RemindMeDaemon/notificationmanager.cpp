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
}

void NotificationManager::initEmpty()
{
	_notifier->setupEmtpy();
}

void NotificationManager::scheduleTriggered(const QUuid &id)
{
	_store->load<Reminder>(id).onResult(this, [this](Reminder rem) {
		_notifier->showNotification(rem);
	}, [this](const QException &e) {
		qCritical() << "Failed to load reminder to display notification with error:" << e.what();
		//TODO error message
	});
}

void NotificationManager::dataChanged(int metaTypeId, const QString &key, bool wasDeleted)
{
	if(metaTypeId == qMetaTypeId<Reminder>() && wasDeleted)
		_notifier->removeNotification(QUuid(key));
}

#include "remindmeservice.h"

RemindmeService::RemindmeService(QObject *parent) :
	QObject(parent),
	_store(nullptr),
	_manager(nullptr),
	_scheduler(new AndroidScheduler(this))
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
		});

		qInfo() << "service successfully started";
		return true;
	} catch(QException &e) {
		qCritical() << e.what();
		return false;
	}
}

void RemindmeService::dataChanged(const QString &key, const QVariant &value)
{
	if(value.isValid()) {
		auto reminder = value.value<Reminder>();
		if(!_scheduler->scheduleReminder(reminder)) {
			//TODO show notification
		}
	} else {
		QUuid id(key);
		_scheduler->cancleReminder(id);
	}
}

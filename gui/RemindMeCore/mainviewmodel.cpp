#include "mainviewmodel.h"

#include <QtMvvmCore/Messages>
#include <QtMvvmDataSyncCore/DataSyncViewModel>

#include "createreminderviewmodel.h"
#include "snoozeviewmodel.h"
#include "datasyncsettingsviewmodel.h"

MainViewModel::MainViewModel(QObject *parent) :
	ViewModel(parent),
	_reminderModel(new QtDataSync::DataStoreModel(this)),
	_sortedModel(new QSortFilterProxyModel(this))
{
	_reminderModel->setTypeId<Reminder>();
	_sortedModel->setSortLocaleAware(true);
	_sortedModel->setSourceModel(_reminderModel);
	_sortedModel->setSortRole(_reminderModel->roleNames().key("current")); //NOTE make settable
	_sortedModel->sort(0);
}

QtDataSync::DataStoreModel *MainViewModel::reminderModel() const
{
	return _reminderModel;
}

QSortFilterProxyModel *MainViewModel::sortedModel() const
{
	return _sortedModel;
}

void MainViewModel::showSettings()
{
	show<DataSyncSettingsViewModel>();
}

void MainViewModel::showSync()
{
	show<QtMvvm::DataSyncViewModel>();
}

void MainViewModel::showAbout()
{
	QtMvvm::about(tr("A simple reminder application for desktop and mobile, with synchronized reminder."),
				  QStringLiteral("https://github.com/Skycoder42/RemindMe"),
				  tr("BSD 3 Clause"),
				  QStringLiteral("https://github.com/Skycoder42/RemindMe/blob/master/LICENSE"));
}

void MainViewModel::addReminder()
{
	show<CreateReminderViewModel>();
}

void MainViewModel::completeReminder(const QUuid &id)
{
	try {
		auto rem = _reminderModel->store()->load<Reminder>(id);
		rem.nextSchedule(_reminderModel->store(), QDateTime::currentDateTime());
	} catch(QException &e) {
		qCritical() << "Failed to complete reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to complete reminder"),
						 tr("Reminder could not be move to the next state. It has most likely become corrupted."));
	}
}

void MainViewModel::deleteReminder(const QUuid &id)
{
	try {
		if(!_reminderModel->store()->remove<Reminder>(id))
			qWarning() << "Reminder with id" << id << "has already been removed";
	} catch(QException &e) {
		qCritical() << "Failed to remove reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to remove reminder"),
						 tr("Reminder could not be deleted from datastore!"));
	}
}

void MainViewModel::snoozeReminder(const QUuid &id)
{
	if(id.isNull())
		return;

	try {
		auto rem = _reminderModel->store()->load<Reminder>(id);
		if(rem.triggerState() == Reminder::Triggered)
			show<SnoozeViewModel>(SnoozeViewModel::showParams(rem));
	} catch (QException &e) {
		qCritical() << "Failed to load reminder with error:" << e.what();
		QtMvvm::critical(tr("Snoozing failed!"),
						 tr("Unable to load the reminder that should be snoozed!"));
	}
}

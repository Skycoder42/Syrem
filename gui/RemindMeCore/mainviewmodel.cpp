#include "mainviewmodel.h"

#include <QtMvvmCore/Messages>
#include <QtMvvmCore/SettingsViewModel>
#include <QtMvvmDataSyncCore/DataSyncViewModel>

#include "createreminderviewmodel.h"

MainViewModel::MainViewModel(QObject *parent) :
	ViewModel(parent),
	_reminderModel(new QtDataSync::DataStoreModel(this))
{
	_reminderModel->setTypeId<Reminder>();
}

QtDataSync::DataStoreModel *MainViewModel::reminderModel() const
{
	return _reminderModel;
}

void MainViewModel::showSettings()
{
	show<QtMvvm::SettingsViewModel>();
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
	Q_UNIMPLEMENTED();
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
	Q_UNIMPLEMENTED();
}

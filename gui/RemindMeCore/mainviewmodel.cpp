#include "mainviewmodel.h"

#include <QtMvvmCore/Messages>
#include <QtMvvmCore/SettingsViewModel>
#include <QtMvvmDataSyncCore/DataSyncViewModel>

MainViewModel::MainViewModel(QObject *parent) :
	ViewModel(parent),
	_reminderModel(nullptr)
{}

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
	Q_UNIMPLEMENTED();
	//show<CreateReminderViewModel>();
}

void MainViewModel::completeReminder(const QUuid &id)
{
	Q_UNIMPLEMENTED();
}

void MainViewModel::deleteReminder(const QUuid &id)
{
	Q_UNIMPLEMENTED();
}

void MainViewModel::snoozeReminder(const QUuid &id)
{
	Q_UNIMPLEMENTED();
}

void MainViewModel::onInit(const QVariantHash &params)
{
	Q_UNUSED(params);

	_reminderModel = new QtDataSync::DataStoreModel(this);
	_reminderModel->setTypeId<Reminder>();
}

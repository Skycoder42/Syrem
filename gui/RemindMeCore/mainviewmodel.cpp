#include "mainviewmodel.h"

#include <QtMvvmCore/Messages>
#include <QtMvvmCore/SettingsViewModel>
#include <QtMvvmDataSyncCore/DataSyncViewModel>

#include "createreminderviewmodel.h"
#include "snoozeviewmodel.h"
#include "datasyncsettingsviewmodel.h"

const QString MainViewModel::paramRemId = QStringLiteral("reminder-id");

MainViewModel::MainViewModel(QObject *parent) :
	ViewModel(parent),
	_reminderModel(new QtDataSync::DataStoreModel(this)),
	_sortedModel(new QSortFilterProxyModel(this))
{
	_reminderModel->setTypeId<Reminder>();
	auto column = _reminderModel->addColumn(tr("Reminder"), "description");
	_reminderModel->addRole(column, Qt::DecorationRole, "important");
	_reminderModel->addRole(column, Qt::DisplayRole, "description");
	column = _reminderModel->addColumn(tr("Due on"), "current");
	_reminderModel->addRole(column, Qt::DecorationRole, "triggerState");
	_reminderModel->addRole(column, Qt::ToolTipRole, "triggerState");

	_sortedModel->setSortLocaleAware(true);
	_sortedModel->setSourceModel(_reminderModel);
	_sortedModel->setSortRole(_reminderModel->roleNames().key("current")); //NOTE make settable
	_sortedModel->sort(0); //TODO implement correct date sorting
}

QVariantHash MainViewModel::showParams(const QUuid &reminderId)
{
	return {
		{paramRemId, reminderId}
	};
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
	show<QtMvvm::SettingsViewModel>(QtMvvm::SettingsViewModel::showParams(SyncedSettings::instance()->accessor())); //TODO get via injection
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
	} catch(QtDataSync::NoDataException &e) {
		qDebug() << "Skipping completing of deleted reminder" << id
				 << "with reason" << e.what();
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
	} catch(QtDataSync::NoDataException &e) {
		qDebug() << "Skipping completing of deleted reminder" << id
				 << "with reason" << e.what();
	} catch (QException &e) {
		qCritical() << "Failed to load reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to snooze reminder"),
						 tr("Unable to load the reminder that should be snoozed!"));
	}
}

void MainViewModel::onInit(const QVariantHash &params)
{
	if(params.contains(paramRemId)) {
		auto remId = params.value(paramRemId).toUuid();
		auto mIndex = _reminderModel->idIndex(remId);
		if(mIndex.isValid())
			emit select(mIndex.row());
	}
}

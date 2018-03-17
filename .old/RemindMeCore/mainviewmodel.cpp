#include "createreminderviewmodel.h"
#include "mainviewmodel.h"
#include "snoozeviewmodel.h"
#include <QtMvvmCore/SettingsViewModel>
#include <QtMvvmCore/Messages>
#include <QtMvvmDataSyncCore/DataSyncViewModel>
#include <QUuid>
#include <rep_remindermanager_replica.h>
#include <remindmeapp.h>

MainViewModel::MainViewModel(QObject *parent) :
	ViewModel(parent),
	_node(new QRemoteObjectNode(this)),
	_reminderManager(nullptr),
	_reminderModel(nullptr)
{
	if(!_node->connectToNode(QUrl(QStringLiteral("local:remindme-daemon")))) {
		qCritical() << "Failed to connect to host node with error:" << _node->lastError();
		return;
	}

	_reminderModel = _node->acquireModel(QStringLiteral("ReminderModel"));
	if(!_reminderModel)
		qCritical() << "Failed to acquire model from node with error:" << _node->lastError();

	_reminderManager = _node->acquire<ReminderManagerReplica>();
	if(!_reminderManager)
		qCritical() << "Failed to acquire manager from node with error:" << _node->lastError();
	connect(_reminderManager, &ReminderManagerReplica::reminderError,
			this, &MainViewModel::reminderError);
}

QAbstractItemModelReplica *MainViewModel::reminderModel() const
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

void MainViewModel::removeReminder(const QUuid &id)
{
	if(id.isNull())
		return;
	_reminderManager->removeReminder(id);
}

void MainViewModel::snoozeReminder(const QUuid &id, quint32 versionCode)
{
	if(id.isNull())
		return;
	coreApp->showSnoozeControl(id, versionCode);
}

void MainViewModel::reminderError(bool isCreate, const QString &error)
{
	if(!isCreate)
		QtMvvm::critical(tr("Failed to remove reminder"), error);
}

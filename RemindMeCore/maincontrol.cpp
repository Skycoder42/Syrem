#include "createremindercontrol.h"
#include "maincontrol.h"
#include "snoozecontrol.h"
#include <settingscontrol.h>
#include <QUuid>
#include <rep_remindermanager_replica.h>
#include <coremessage.h>
#include <remindmeapp.h>

MainControl::MainControl(QObject *parent) :
	Control(parent),
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
			this, &MainControl::reminderError);
}

void MainControl::onShow()
{
	qDebug("Main gui is now visible");
	//logic to execute when the gui is shown
}

void MainControl::onClose()
{
	qDebug("Main gui is now closed");
	//logic to execute when the gui was closed
}

QAbstractItemModelReplica *MainControl::reminderModel() const
{
	return _reminderModel;
}

void MainControl::showSettings()
{
	auto settings = new SettingsControl(this);
	settings->setDeleteOnClose(true);
	settings->show();
}

void MainControl::addReminder()
{
	auto addRem = new CreateReminderControl(this);
	addRem->setDeleteOnClose(true);
	addRem->show();
}

void MainControl::removeReminder(const QUuid &id)
{
	if(id.isNull())
		return;
	_reminderManager->removeReminder(id);
}

void MainControl::snoozeReminder(const QUuid &id, quint32 versionCode)
{
	if(id.isNull())
		return;
	coreApp->showSnoozeControl(id, versionCode);
}

void MainControl::reminderError(bool isCreate, const QString &error)
{
	if(!isCreate)
		CoreMessage::critical(tr("Failed to remove reminder"), error);
}

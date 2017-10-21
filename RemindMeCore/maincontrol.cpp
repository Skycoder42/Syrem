#include "createremindercontrol.h"
#include "maincontrol.h"
#include <settingscontrol.h>

MainControl::MainControl(QObject *parent) :
	Control(parent),
	_node(new QRemoteObjectNode(this)),
	_reminderModel(nullptr)
{
	if(!_node->connectToNode(QUrl(QStringLiteral("local:remindme-daemon")))) {
		qCritical() << "Failed to connect to host node with error:" << _node->lastError();
		return;
	}

	_reminderModel = _node->acquireModel(QStringLiteral("ReminderModel"));
	if(!_reminderModel)
		qCritical() << "Failed to aquire model from node with error:" << _node->lastError();

	//DEBUG
	connect(_reminderModel, &QAbstractItemModelReplica::initialized, this, [](){
		qDebug() << "ReminderModel initialized";
	});
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

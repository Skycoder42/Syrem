#include "remindmedaemon.h"
#include <QRemoteObjectHost>
#include <QCoreApplication>
#include <QJsonSerializer>
#include <QtDataSync/Setup>
#include <QtDataSync/WsAuthenticator>
#include <QtDataSync/DataStoreModel>

#include "remindermanager.h"
#include "reminder.h"

#ifndef DATASYNC_SERVER_SECRET
#define DATASYNC_SERVER_SECRET "debug-secret"
#endif

using namespace QtDataSync;

RemindMeDaemon::RemindMeDaemon(QObject *parent) :
	QObject(parent),
	_hostNode(new QRemoteObjectHost(this)),
	_storeModel(nullptr),
	_manager(new ReminderManager(this))
{
	QJsonSerializer::registerAllConverters<Reminder>();
}

void RemindMeDaemon::startDaemon()
{
	_hostNode->setName(QStringLiteral("daemon"));
	if(!_hostNode->setHostUrl(QUrl(QStringLiteral("local:remindme-daemon")))) {
		qCritical() << "Failed to create host node with error:" << _hostNode->lastError();
		return;
	}

	Setup setup;
	setup.create();
	auto auth = Setup::authenticatorForSetup<QtDataSync::WsAuthenticator>(this);
	if(!auth->remoteUrl().isValid()) {
		auth->setRemoteUrl(QStringLiteral("wss://apps.skycoder42.de/remind-me/"));
		auth->setServerSecret(QString::fromUtf8(DATASYNC_SERVER_SECRET));
		auth->reconnect();
	}

	_storeModel = new DataStoreModel(this);
	_storeModel->setTypeId<Reminder>();
	if(!_hostNode->enableRemoting(_storeModel,
								  QStringLiteral("ReminderModel"),
								  _storeModel->roleNames().keys().toVector())) {
		qCritical() << "Failed to expose DataStoreModel with error:" << _hostNode->lastError();
	}

	if(!_hostNode->enableRemoting(_manager)) {
		qCritical() << "Failed to expose ReminderManager with error:" << _hostNode->lastError();
	}

	qDebug() << "daemon started";
}

void RemindMeDaemon::commandMessage(const QStringList &message)
{
	if(!message.contains(QStringLiteral("--daemon")))
		return;

	if(message.contains(QStringLiteral("--quit"))) {
		qInfo() << "Received quit command, stopping";
		qApp->quit();
	}
}

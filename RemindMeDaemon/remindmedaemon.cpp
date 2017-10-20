#include "remindmedaemon.h"
#include <QRemoteObjectHost>
#include <QCoreApplication>

#include "remindermanager.h"

RemindMeDaemon::RemindMeDaemon(QObject *parent) :
	QObject(parent)
{}

void RemindMeDaemon::startDaemon()
{
	_hostNode = new QRemoteObjectHost(this);
	_hostNode->setName(QStringLiteral("daemon"));
	if(!_hostNode->setHostUrl(QUrl(QStringLiteral("local:remindme-daemon")))) {
		qCritical() << _hostNode->lastError();
		return;
	}

	_manager = new ReminderManager(this);
	if(!_hostNode->enableRemoting(_manager)) {
		qCritical() << _hostNode->lastError();
		return;
	}

	qDebug() << "daemon started";
}

void RemindMeDaemon::commandMessage(const QStringList &message)
{
	if(message.contains(QStringLiteral("--quit"))) {
		qInfo() << "Received quit command, stopping";
		qApp->quit();
	}
}

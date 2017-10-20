#include "remindmedaemon.h"
#include <QRemoteObjectHost>

#include "remindermanager.h"

RemindMeDaemon::RemindMeDaemon(QObject *parent) :
	QObject(parent)
{}

void RemindMeDaemon::setupDaemon()
{
	_hostNode = new QRemoteObjectHost(this);
	_hostNode->setName(QStringLiteral("daemon"));
	if(!_hostNode->setHostUrl(QUrl(QStringLiteral("local:remindme")))) {
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

#include "remindmedaemon.h"
#include <QRemoteObjectHost>
#include <QCoreApplication>
#include <QJsonSerializer>
#include <QtDataSync/Setup>
#include <QtDataSync/DataStoreModel>
#include <QtDataSync/ConflictResolver>

#include "remindermanager.h"
#include "snoozehelper.h"
#include "reminder.h"
#include "notificationmanager.h"

#include "../../__private/remindme-datasync-secret.h"
#ifndef DATASYNC_SERVER_SECRET
#define DATASYNC_SERVER_SECRET "debug-secret"
#endif

using namespace QtDataSync;

RemindMeDaemon::RemindMeDaemon(QObject *parent) :
	QObject(parent),
	_hostNode(nullptr),
	_storeModel(nullptr),
	_remManager(nullptr),
	_snoozeHelper(nullptr),
	_notManager(nullptr)
{
	qRegisterMetaType<QList<QPair<int, ParserTypes::Expression::Span>>>("QList<QPair<int,ParserTypes::Expression::Span>>");
	qRegisterMetaType<ParserTypes::Datum*>();
	qRegisterMetaType<ParserTypes::Type*>();
	qRegisterMetaType<ParserTypes::TimePoint*>();
	qRegisterMetaType<Schedule*>();
	qRegisterMetaType<OneTimeSchedule*>();
	qRegisterMetaType<LoopSchedule*>();
	qRegisterMetaType<MultiSchedule*>();

	QJsonSerializer::registerAllConverters<Reminder>();
	QJsonSerializer::registerPointerConverters<Schedule>();
	QJsonSerializer::registerPairConverters<int, ParserTypes::Expression::Span>();
	QJsonSerializer::registerAllConverters<QPair<int, ParserTypes::Expression::Span>>();
}

void RemindMeDaemon::startDaemon()
{
	//basic setup
	_hostNode = new QRemoteObjectHost(this);
	_hostNode->setName(QStringLiteral("daemon"));
	if(!_hostNode->setHostUrl(QUrl(QStringLiteral("local:remindme-daemon")))) {
		qCritical() << "Failed to create host node with error:" << _hostNode->lastError();
		return;
	}

	Setup().setRemoteObjectHost(QStringLiteral("local:remindme-datasync")) //TODO make SSL with pw
			.setSyncPolicy(Setup::PreferDeleted)
			.setRemoteConfiguration({QStringLiteral("wss://apps.skycoder42.de/datasync/")})
			//TODO custom conflict resolver
			.create();

	//exposed classes
	_storeModel = new DataStoreModel(this);
	connect(_storeModel, &DataStoreModel::storeError, this, [](const QException &e) {
		qCritical() << "Failed to load DataStoreModel with error:" << e.what();
	});
	_storeModel->setTypeId<Reminder>();
	if(!_hostNode->enableRemoting(_storeModel,
								  QStringLiteral("ReminderModel"),
								  _storeModel->roleNames().keys().toVector()))
		qCritical() << "Failed to expose DataStoreModel with error:" << _hostNode->lastError();

	_remManager = new ReminderManager(this);
	if(!_hostNode->enableRemoting(_remManager))
		qCritical() << "Failed to expose ReminderManager with error:" << _hostNode->lastError();
	_snoozeHelper = new SnoozeHelper(this);
	if(!_hostNode->enableRemoting(_snoozeHelper))
		qCritical() << "Failed to expose SnoozeHelper with error:" << _hostNode->lastError();

	//unexposed classes
	_notManager = new NotificationManager(this);

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

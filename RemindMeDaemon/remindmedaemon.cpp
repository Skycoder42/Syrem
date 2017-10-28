#include "remindmedaemon.h"
#include <QRemoteObjectHost>
#include <QCoreApplication>
#include <QJsonSerializer>
#include <QtDataSync/Setup>
#include <QtDataSync/WsAuthenticator>
#include <QtDataSync/DataStoreModel>
#include <QtDataSync/DataMerger>

#include "remindermanager.h"
#include "snoozehelper.h"
#include "reminder.h"
#include "notificationmanager.h"

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

	Setup setup;
	setup.serializer()->addJsonTypeConverter(new DateTimeJsonConverter());
	setup.dataMerger()->setSyncPolicy(DataMerger::PreferDeleted);
	setup.create();
	auto auth = Setup::authenticatorForSetup<QtDataSync::WsAuthenticator>(this);
	if(!auth->remoteUrl().isValid()) {
		auth->setRemoteUrl(QStringLiteral("wss://apps.skycoder42.de/remind-me/"));
		auth->setServerSecret(QString::fromUtf8(DATASYNC_SERVER_SECRET));
		auth->reconnect();
	}

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



bool DateTimeJsonConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QDateTime ||
			metaTypeId == QMetaType::QDate ||
			metaTypeId == QMetaType::QTime;
}

QList<QJsonValue::Type> DateTimeJsonConverter::jsonTypes() const
{
	return {QJsonValue::String};
}

QJsonValue DateTimeJsonConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(helper)
	switch (propertyType) {
	case QMetaType::QDateTime:
		return value.toDateTime().toString(Qt::ISODate);
	case QMetaType::QDate:
		return value.toDate().toString(Qt::ISODate);
	case QMetaType::QTime:
		return value.toTime().toString(Qt::ISODate);
	default:
		throw QJsonSerializationException("unsupported type");
	}
}

QVariant DateTimeJsonConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(parent)
	Q_UNUSED(helper)
	switch (propertyType) {
	case QMetaType::QDateTime:
		return QDateTime::fromString(value.toString(), Qt::ISODate);
	case QMetaType::QDate:
		return QDate::fromString(value.toString(), Qt::ISODate);
	case QMetaType::QTime:
		return QTime::fromString(value.toString(), Qt::ISODate);
	default:
		throw QJsonSerializationException("unsupported type");
	}
}

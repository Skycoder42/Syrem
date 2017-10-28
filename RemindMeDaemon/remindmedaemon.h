#ifndef REMINDMEDAEMON_H
#define REMINDMEDAEMON_H

#include <QObject>
#include <QJsonTypeConverter>
class QRemoteObjectHost;
class ReminderManager;
class SnoozeHelper;
class NotificationManager;

namespace QtDataSync {
class DataStoreModel;
}

class DateTimeJsonConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;
};

class RemindMeDaemon : public QObject
{
	Q_OBJECT

public:
	RemindMeDaemon(QObject *parent = nullptr);

public slots:
	void startDaemon();

	void commandMessage(const QStringList &message);

private:
	QRemoteObjectHost *_hostNode;

	QtDataSync::DataStoreModel *_storeModel;
	ReminderManager *_remManager;
	SnoozeHelper *_snoozeHelper;
	NotificationManager *_notManager;
};

#endif // REMINDMEDAEMON_H

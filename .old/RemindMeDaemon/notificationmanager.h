#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QtDataSync/SyncManager>
#include <QtDataSync/DataTypeStore>
#include <ischeduler.h>
#include <inotifier.h>
#include <QSettings>

class NotificationManager : public QObject
{
	Q_OBJECT

public:
	explicit NotificationManager(QObject *parent = nullptr);

private slots:
	void scheduleTriggered(const QUuid &id);

	void messageDismissed(const QUuid &id, quint32 versionCode);
	void messageCompleted(const QUuid &id, quint32 versionCode);
	void messageDelayed(const QUuid &id, quint32 versionCode, QDateTime nextTrigger);

	void dataChanged(const QString &key, const QVariant &value);

private:
	IScheduler *_scheduler;
	INotifier *_notifier;

	QSettings *_settings;
	QtDataSync::SyncManager *_manager;
	QtDataSync::DataTypeStore<Reminder, QUuid> *_store;
};

#endif // NOTIFICATIONMANAGER_H

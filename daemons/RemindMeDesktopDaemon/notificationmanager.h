#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QtDataSync/SyncManager>
#include <QtDataSync/DataTypeStore>
#include <QtMvvmCore/Injection>
#include <timerscheduler.h>
#include <inotifier.h>
#include <syncedsettings.h>

class NotificationManager : public QObject
{
	Q_OBJECT

	QTMVVM_INJECT_PROP(INotifier*, notifier, _notifier)
	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	Q_INVOKABLE explicit NotificationManager(QObject *parent = nullptr);

public slots:
	void init();

private slots:
	void scheduleTriggered(const QUuid &id);

	void messageCompleted(const QUuid &id, quint32 versionCode);
	void messageDelayed(const QUuid &id, quint32 versionCode, QDateTime nextTrigger);
	void messageActivated(const QUuid &id);

	void dataChanged(const QString &key, const QVariant &value);

private:
	TimerScheduler *_scheduler;
	INotifier *_notifier;
	SyncedSettings *_settings;

	QtDataSync::SyncManager *_manager;
	QtDataSync::DataTypeStore<Reminder, QUuid> *_store;
};

#endif // NOTIFICATIONMANAGER_H

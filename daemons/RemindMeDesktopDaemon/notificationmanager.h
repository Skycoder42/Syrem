#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QtDataSync/SyncManager>
#include <QtDataSync/DataTypeStore>
#include <QtMvvmCore/Injection>
#include <qtaskbarcontrol.h>
#include <syncedsettings.h>
#include "timerscheduler.h"
#include "inotifier.h"

class NotificationManager : public QObject
{
	Q_OBJECT

	QTMVVM_INJECT_PROP(INotifier*, notifier, _notifier)
	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	Q_INVOKABLE explicit NotificationManager(QObject *parent = nullptr);

public slots:
	void init();
	void triggerSync();

private slots:
	void scheduleTriggered(const QUuid &id);

	void messageCompleted(const QUuid &id, quint32 versionCode);
	void messageDelayed(const QUuid &id, quint32 versionCode, QDateTime nextTrigger);
	void messageActivated(const QUuid &id);

	void dataChanged(const QString &key, const QVariant &value);

private:
	TimerScheduler *_scheduler;
	QTaskbarControl *_taskbar;
	INotifier *_notifier;
	SyncedSettings *_settings;

	QtDataSync::SyncManager *_manager;
	QtDataSync::DataTypeStore<Reminder, QUuid> *_store;

	void updateNotificationCount(int increment);
};

#endif // NOTIFICATIONMANAGER_H

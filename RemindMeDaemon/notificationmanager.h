#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QtDataSync/AsyncDataStore>
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

	void messageDismissed(Reminder reminder);
	void messageCompleted(Reminder reminder);
	void messageDelayed(Reminder reminder, const QDateTime &nextTrigger);

	void dataChanged(int metaTypeId, const QString &key, bool wasDeleted);

private:
	IScheduler *_scheduler;
	INotifier *_notifier;

	QSettings *_settings;
	QtDataSync::AsyncDataStore *_store;

	void doSchedule(const Reminder &reminder);
};

#endif // NOTIFICATIONMANAGER_H

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QtDataSync/AsyncDataStore>
#include <ischeduler.h>
#include <inotifier.h>

class NotificationManager : public QObject
{
	Q_OBJECT

public:
	explicit NotificationManager(QObject *parent = nullptr);

public slots:
	void initEmpty();

private slots:
	void scheduleTriggered(const QUuid &id);

	void dataChanged(int metaTypeId, const QString &key, bool wasDeleted);

private:
	IScheduler *_scheduler;
	INotifier *_notifier;

	QtDataSync::AsyncDataStore *_store;
};

#endif // NOTIFICATIONMANAGER_H

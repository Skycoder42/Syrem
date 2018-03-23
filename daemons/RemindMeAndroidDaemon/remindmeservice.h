#ifndef REMINDMESERVICE_H
#define REMINDMESERVICE_H

#include <QObject>
#include <QtDataSync/SyncManager>
#include <remindmelib.h>

#include "androidscheduler.h"

class RemindmeService : public QObject
{
	Q_OBJECT

public:
	explicit RemindmeService(QObject *parent = nullptr);

	bool startService();

private slots:
	void dataChanged(const QString &key, const QVariant &value);

private:
	ReminderStore *_store;
	QtDataSync::SyncManager *_manager;

	AndroidScheduler *_scheduler;
};

#endif // REMINDMESERVICE_H

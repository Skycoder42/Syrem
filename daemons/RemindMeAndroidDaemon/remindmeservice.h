#ifndef REMINDMESERVICE_H
#define REMINDMESERVICE_H

#include <QObject>
#include <QPointer>
#include <QMutex>
#include <QtDataSync/SyncManager>
#include <remindmelib.h>

#include "androidscheduler.h"

class RemindmeService : public QObject
{
	Q_OBJECT

public:
	struct Intent {
		QString action;
		QUuid reminderId;
		quint32 versionCode;
		QString result;
	};

	explicit RemindmeService(QObject *parent = nullptr);

	bool startService();

	static void handleIntent(const Intent &intent);

private slots:
	void dataChanged(const QString &key, const QVariant &value);

	void handleAllIntents();
	void tryQuit();

private:
	ReminderStore *_store;
	QtDataSync::SyncManager *_manager;

	AndroidScheduler *_scheduler;

	static QMutex _runMutex;
	static QPointer<RemindmeService> _runInstance;
	static QList<Intent> _currentIntents;
};

#endif // REMINDMESERVICE_H

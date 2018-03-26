#ifndef REMINDMESERVICE_H
#define REMINDMESERVICE_H

#include <QObject>
#include <QPointer>
#include <QMutex>
#include <QtDataSync/SyncManager>
#include <remindmelib.h>
#include <dateparser.h>

#include "androidscheduler.h"
#include "androidnotifier.h"

class RemindmeService : public QObject //TODO use service registry more?
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

	void actionSchedule(const QUuid &id, quint32 versionCode);
	void actionComplete(const QUuid &id, quint32 versionCode);
	void actionSnooze(const QUuid &id, quint32 versionCode, const QString &expression);

private:
	static const QString ActionScheduler;
	static const QString ActionComplete;
	static const QString ActionSnooze;

	ReminderStore *_store;
	QtDataSync::SyncManager *_manager;
	DateParser *_parser;

	AndroidScheduler *_scheduler;
	AndroidNotifier *_notifier;

	static QMutex _runMutex;
	static QPointer<RemindmeService> _runInstance;
	static QList<Intent> _currentIntents;
};

#endif // REMINDMESERVICE_H

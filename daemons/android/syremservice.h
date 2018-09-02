#ifndef SYREMSERVICE_H
#define SYREMSERVICE_H

#include <QObject>
#include <QPointer>
#include <QMutex>
#include <QtDataSync/SyncManager>
#include <QtService/Service>
#include <libsyrem.h>
#include <eventexpressionparser.h>
#include <QAndroidIntent>

#include "androidscheduler.h"
#include "androidnotifier.h"

class SyremService : public QtService::Service //TODO use service registry more?
{
	Q_OBJECT

public:
	struct Intent {
		QString action;
		QUuid reminderId;
		quint32 versionCode;
		QString result;
	};

	explicit SyremService(int &argc, char **argv);

	static void handleIntent(const Intent &intent); //TODO make private, non static

private slots:
	void dataResetted();
	void dataChanged(const QString &key, const QVariant &value);

	void handleAllIntents();
	void tryQuit();

	void actionSchedule(const QUuid &id, quint32 versionCode);
	void actionComplete(const QUuid &id, quint32 versionCode);
	void actionSnooze(const QUuid &id, quint32 versionCode, const QString &expression);
	void actionSetup();

protected:
	CommandMode onStart() override;

private:
	static const QString ActionScheduler;
	static const QString ActionComplete;
	static const QString ActionSnooze;
	static const QString ActionRefresh;
	static const QString ActionSetup;

	ReminderStore *_store = nullptr;
	QtDataSync::SyncManager *_manager = nullptr;
	EventExpressionParser *_parser = nullptr;

	AndroidScheduler *_scheduler = nullptr;
	AndroidNotifier *_notifier = nullptr;

	static QMutex _runMutex;
	static QPointer<SyremService> _runInstance;
	static QList<Intent> _currentIntents;

	void doSchedule(const Reminder &reminder);

	void addNotify(const QUuid &id);
	void removeNotify(const QUuid &id);
	void updateNotificationCount(int count);

	int onStartCommand(const QAndroidIntent &intent, int flags, int startId);
};

Q_DECLARE_METATYPE(QAndroidIntent)

#endif // SYREMSERVICE_H

#ifndef ANDROIDNOTIFIER_H
#define ANDROIDNOTIFIER_H

#include <QObject>
#include <QMutex>
#include <QUuid>
#include <QSet>
#include <inotifier.h>
#include <tuple>

class AndroidNotifier : public QObject, public INotifier
{
	Q_OBJECT
	Q_INTERFACES(INotifier)

public:
	static const QString ActionScheduler;
	static const QString ActionComplete;
	static const QString ActionDismiss;
	static const QString ActionSnooze;

	Q_INVOKABLE explicit AndroidNotifier(QObject *parent = nullptr);

	static void guiStarted();
	static void serviceStarted();

	static void handleServiceIntent(const QString &action, const QUuid &id, quint32 versionCode, const QString &result);
	static void handleActivityIntent(const QString &action, const QUuid &id, quint32 versionCode);

public slots:
	void beginSetup() override;
	void endSetup() override;
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;
	void showErrorMessage(const QString &error) override;
	void notificationHandled(const QUuid &id, const QString &errorMsg) override;

signals:
	void messageDismissed(const QUuid &id, quint32 versionCode) final;
	void messageCompleted(const QUuid &id, quint32 versionCode) final;
	void messageDelayed(const QUuid &id, quint32 versionCode, const QDateTime &nextTrigger) final;

private slots:
	void handleIntentImpl();

private:
	typedef std::tuple<QString, QUuid, quint32, QString> Intent;

	static bool _canInvoke;
	static QMutex _invokeMutex;
	static QList<Intent> _intentCache;
	static QSet<QUuid> _blockList;

	bool _setup;
	QSet<QUuid> _setupIds;
	QSet<QUuid> _actionIds;
	bool _shouldSync;

	QDateTime parseExpression(const QString &expression);
	void tryQuit();
};

#endif // ANDROIDNOTIFIER_H
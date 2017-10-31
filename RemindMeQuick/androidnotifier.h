#ifndef ANDROIDNOTIFIER_H
#define ANDROIDNOTIFIER_H

#include <QObject>
#include <inotifier.h>
#include <tuple>

class AndroidNotifier : public QObject, public INotifier
{
	Q_OBJECT
	Q_INTERFACES(INotifier)

public:
	explicit AndroidNotifier(QObject *parent = nullptr);

	static void handleIntent(const QString &action, const QUuid &id, quint32 versionCode);

public slots:
	void beginSetup() override;
	void endSetup() override;
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;
	void showErrorMessage(const QString &error) override;

	void handleIntentImpl();

signals:
	void messageDismissed(Reminder reminder) final;
	void messageCompleted(Reminder reminder) final;
	void messageDelayed(Reminder reminder, const QDateTime &nextTrigger) final;

private:
	typedef std::tuple<QString, QUuid, quint32> Intent;

	static bool _canInvoke;
	static QMutex _invokeMutex;
	static QList<Intent> _intentCache;

	bool _setup;
	QList<QUuid> _setupIds;
};

#endif // ANDROIDNOTIFIER_H

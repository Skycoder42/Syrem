#ifndef ANDROIDNOTIFIER_H
#define ANDROIDNOTIFIER_H

#include <QObject>
#include <inotifier.h>

class AndroidNotifier : public QObject, public INotifier
{
	Q_OBJECT
	Q_INTERFACES(INotifier)

public:
	explicit AndroidNotifier(QObject *parent = nullptr);

	static void initIntent(const QString &action, const QString &data);

public slots:
	void beginSetup() override;
	void endSetup() override;
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;
	void showErrorMessage(const QString &error) override;

	void reactToStart();

signals:
	void messageDismissed(Reminder reminder) final;
	void messageCompleted(Reminder reminder) final;
	void messageDelayed(Reminder reminder, const QDateTime &nextTrigger) final;

private:
	static bool _canInvoke;
	static QMutex _invokeMutex;
	static QList<QPair<QString, QString>> _startServiceCache;

	bool _setup;
	QList<QUuid> _setupIds;
};

#endif // ANDROIDNOTIFIER_H

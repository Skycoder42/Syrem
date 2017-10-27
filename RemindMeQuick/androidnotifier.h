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

public slots:
	void beginSetup() override;
	void endSetup() override;
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;
	void showErrorMessage(const QString &error) override;

signals:
	void messageDismissed(Reminder reminder) final;
	void messageCompleted(Reminder reminder) final;
	void messageDelayed(Reminder reminder, const QDateTime &nextTrigger) final;
};

#endif // ANDROIDNOTIFIER_H

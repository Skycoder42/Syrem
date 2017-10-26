#ifndef INOTIFIER_H
#define INOTIFIER_H

#include <QObject>
#include <QUuid>
#include "reminder.h"

class INotifier
{
public:
	virtual inline ~INotifier() = default;

public slots:
	virtual void beginSetup() = 0;
	virtual void endSetup() = 0;

	virtual void showNotification(const Reminder &reminder) = 0;
	virtual void removeNotification(const QUuid &id) = 0;

	virtual void showErrorMessage(const QString &error) = 0;

signals:
	virtual void messageDismissed(Reminder reminder) = 0;
	virtual void messageCompleted(Reminder reminder) = 0;
	virtual void messageDelayed(Reminder reminder, const QDateTime &nextTrigger) = 0;
};

#define INotifier_iid "de.skycoder42.remindme.daemon.INotifier"
Q_DECLARE_INTERFACE(INotifier, INotifier_iid)

#endif // INOTIFIER_H

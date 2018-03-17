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

	virtual void notificationHandled(const QUuid &id, const QString &errorMsg = {}) = 0;

signals:
	virtual void messageDismissed(const QUuid &id, quint32 versionCode) = 0;
	virtual void messageCompleted(const QUuid &id, quint32 versionCode) = 0;
	virtual void messageDelayed(const QUuid &id, quint32 versionCode, const QDateTime &nextTrigger) = 0;
};

#define INotifier_iid "de.skycoder42.remindme.daemon.INotifier"
Q_DECLARE_INTERFACE(INotifier, INotifier_iid)
Q_DECLARE_METATYPE(INotifier*)

#endif // INOTIFIER_H

#ifndef INOTIFIER_H
#define INOTIFIER_H

#include <QObject>
#include <QUuid>
#include <QLoggingCategory>
#include <reminder.h>

class INotifier
{
public:
	virtual inline ~INotifier() = default;

public slots:
	virtual void showNotification(const Reminder &reminder) = 0;
	virtual void removeNotification(const QUuid &id) = 0;

	virtual void showErrorMessage(const QString &error) = 0;

	virtual void cancelAll() = 0;

signals:
	virtual void messageCompleted(const QUuid &id, quint32 versionCode) = 0;
	virtual void messageDelayed(const QUuid &id, quint32 versionCode, const QDateTime &nextTrigger) = 0;
	virtual void messageActivated(const QUuid &id) = 0;
};

#define INotifier_iid "de.skycoder42.remindme.daemon.INotifier"
Q_DECLARE_INTERFACE(INotifier, INotifier_iid)
Q_DECLARE_METATYPE(INotifier*)

Q_DECLARE_LOGGING_CATEGORY(notifier)

#endif // INOTIFIER_H

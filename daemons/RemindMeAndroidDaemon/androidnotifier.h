#ifndef ANDROIDNOTIFIER_H
#define ANDROIDNOTIFIER_H

#include <QObject>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <reminder.h>

class AndroidNotifier : public QObject
{
	Q_OBJECT

public:
	explicit AndroidNotifier(QObject *parent = nullptr);

public slots:
	void showNotification(const Reminder &reminder);
	void showParserError(const Reminder &reminder, const QString &errorText);
	void removeNotification(const QUuid &id);
	void showErrorMessage(const QString &error);

	void cleanNotifications();

private:
	QAndroidJniObject _jNotifier;

	QAndroidJniObject createSnoozeArray(QAndroidJniEnvironment &env);
};

#endif // ANDROIDNOTIFIER_H

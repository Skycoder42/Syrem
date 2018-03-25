#ifndef ANDROIDNOTIFIER_H
#define ANDROIDNOTIFIER_H

#include <QObject>
#include <QAndroidJniObject>
#include <reminder.h>

class AndroidNotifier : public QObject
{
	Q_OBJECT

public:
	explicit AndroidNotifier(QObject *parent = nullptr);

public slots:
	void showNotification(const Reminder &reminder);
	void removeNotification(const QUuid &id);
	void showErrorMessage(const QString &error);

private:
	QAndroidJniObject _jNotifier;
};

#endif // ANDROIDNOTIFIER_H

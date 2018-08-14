#ifndef REMINDMEDAEMON_H
#define REMINDMEDAEMON_H

#include <QObject>
class NotificationManager;

class RemindMeDaemon : public QObject
{
	Q_OBJECT

public:
	RemindMeDaemon(QObject *parent = nullptr);

	bool startDaemon(bool systemdLog);

private slots:
	void signalTriggered(int sig);

private:
	NotificationManager *_notManager;
};

#endif // REMINDMEDAEMON_H

#ifndef REMINDMEDAEMON_H
#define REMINDMEDAEMON_H

#include <QObject>
#include <sys/syslog.h>
class NotificationManager;

namespace QtDataSync {
class DataStoreModel;
}

class RemindMeDaemon : public QObject
{
	Q_OBJECT

public:
	RemindMeDaemon(QObject *parent = nullptr);

	bool startDaemon(bool systemdLog);

private:
	NotificationManager *_notManager;
};

#endif // REMINDMEDAEMON_H

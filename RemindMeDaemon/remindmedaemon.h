#ifndef REMINDMEDAEMON_H
#define REMINDMEDAEMON_H

#include <QObject>
class QRemoteObjectHost;
class ReminderManager;

namespace QtDataSync {
class DataStoreModel;
}

class RemindMeDaemon : public QObject
{
	Q_OBJECT

public:
	RemindMeDaemon(QObject *parent = nullptr);

public slots:
	void startDaemon();

	void commandMessage(const QStringList &message);

private:
	QRemoteObjectHost *_hostNode;

	QtDataSync::DataStoreModel *_storeModel;
	ReminderManager *_manager;
};

#endif // REMINDMEDAEMON_H

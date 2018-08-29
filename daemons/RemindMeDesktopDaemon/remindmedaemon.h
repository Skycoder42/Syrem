#ifndef REMINDMEDAEMON_H
#define REMINDMEDAEMON_H

#include <QtMvvmCore/CoreApp>
class NotificationManager;

class RemindMeDaemon : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	RemindMeDaemon(QObject *parent = nullptr);

	bool startDaemon(bool systemdLog);

protected:
	int startApp(const QStringList &arguments) override;

private slots:
	void signalTriggered(int sig);

private:
	NotificationManager *_notManager = nullptr;
};

#endif // REMINDMEDAEMON_H

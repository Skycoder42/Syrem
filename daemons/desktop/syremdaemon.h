#ifndef SYREMDAEMON_H
#define SYREMDAEMON_H

#include <QtMvvmCore/CoreApp>
class NotificationManager;

class SyremDaemon : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	SyremDaemon(QObject *parent = nullptr);

	bool startDaemon(bool systemdLog);

protected:
	int startApp(const QStringList &arguments) override;

private slots:
	void signalTriggered(int sig);

private:
	NotificationManager *_notManager = nullptr;
};

#endif // SYREMDAEMON_H

#ifndef DAEMONCONTROLLER_H
#define DAEMONCONTROLLER_H

#include <QObject>
#include <QProcess>

class DaemonController : public QObject
{
	Q_OBJECT

public:
	explicit DaemonController(QObject *parent = nullptr);

	void ensureStarted();
	void restart();
	void stop();

private:
	QProcess *_process;
};

#endif // DAEMONCONTROLLER_H

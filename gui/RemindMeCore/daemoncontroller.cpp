#include "daemoncontroller.h"

#include <QCoreApplication>

DaemonController::DaemonController(QObject *parent) :
	QObject(parent),
	_process(new QProcess(this))
{
	_process->setProgram(QCoreApplication::applicationDirPath() +
						 QStringLiteral("/../../daemons/RemindMeDesktopDaemon/remind-med"));
	_process->setProcessChannelMode(QProcess::ForwardedChannels);
}

void DaemonController::ensureStarted()
{
	//TODO debug implementation
	_process->start();
}

void DaemonController::restart()
{
	//TODO debug implementation
	Q_UNIMPLEMENTED();
}

void DaemonController::stop()
{
	//TODO debug implementation
	_process->terminate();
	if(!_process->waitForFinished(5000))
		_process->kill();
}

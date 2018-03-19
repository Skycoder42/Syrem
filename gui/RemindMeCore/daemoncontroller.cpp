#include "daemoncontroller.h"

#include <QCoreApplication>

DaemonController::DaemonController(QObject *parent) :
	QObject(parent),
	_process(new QProcess(this))
{
	//TODO debug implementation
	_process->setProgram(QCoreApplication::applicationDirPath() +
						 QStringLiteral("/../../daemons/RemindMeDesktopDaemon/remind-med"));
	_process->setProcessChannelMode(QProcess::ForwardedChannels);
}

void DaemonController::ensureStarted()
{
	_process->start();
}

void DaemonController::restart()
{
	Q_UNIMPLEMENTED();
}

void DaemonController::stop()
{
	_process->terminate();
	if(!_process->waitForFinished(5000))
		_process->kill();
}

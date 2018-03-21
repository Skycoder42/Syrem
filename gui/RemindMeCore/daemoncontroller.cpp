#include "daemoncontroller.h"

#include <QCoreApplication>
#include <QStandardPaths>

DaemonController::DaemonController(QObject *parent) :
	QObject(parent)
#ifdef NEEDS_QPROCESS
	,_process(new QProcess(this))
#endif
{
#ifndef QT_NO_DEBUG
	_process->setProgram(QCoreApplication::applicationDirPath() +
						 QStringLiteral("/../../daemons/RemindMeDesktopDaemon/remind-med"));
	_process->setProcessChannelMode(QProcess::ForwardedChannels);
#elif defined(NEEDS_QPROCESS)
	_process->setProgram(QStandardPaths::findExecutable(QStringLiteral("remind-med")));
#endif
}

void DaemonController::ensureStarted()
{
#ifdef NEEDS_QPROCESS
	_process->start();
#else
	runSystemCtl(QStringLiteral("start"));
#endif
}

void DaemonController::restart()
{
#ifdef NEEDS_QPROCESS
	stop();
	ensureStarted();
#else
	runSystemCtl(QStringLiteral("restart"));
#endif
}

void DaemonController::stop()
{
#ifdef NEEDS_QPROCESS
	_process->terminate();
	if(!_process->waitForFinished(5000))
		_process->kill();
#else
	runSystemCtl(QStringLiteral("stop"));
#endif
}

#ifndef NEEDS_QPROCESS
bool DaemonController::runSystemCtl(const QString &command) const
{
	auto systemCtl = QStandardPaths::findExecutable(QStringLiteral("systemctl"));
	if(systemCtl.isEmpty())
		return false;
	QStringList args  {
		QStringLiteral("--user"),
		command,
		QStringLiteral("remind-me.service")
	};
	return QProcess::execute(systemCtl, args) == EXIT_SUCCESS;
}
#endif

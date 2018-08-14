#include "remindmedaemon.h"
#include <QCommandLineParser>
#include <QtMvvmCore/ServiceRegistry>
#include <QCtrlSignals>
#include <remindmelib.h>

#include "notificationmanager.h"

#include <QtCore/private/qtcore-config_p.h>

using namespace QtDataSync;

RemindMeDaemon::RemindMeDaemon(QObject *parent) :
	QObject(parent),
	_notManager(nullptr)
{}

bool RemindMeDaemon::startDaemon(bool systemdLog)
{
	if(systemdLog) {
#if !QT_CONFIG(journald) && !QT_CONFIG(syslog)
		qSetMessagePattern(QStringLiteral("%{if-fatal}<0>%{endif}"
										  "%{if-critical}<2>%{endif}"
										  "%{if-warning}<4>%{endif}"
										  "%{if-info}<6>%{endif}"
										  "%{if-debug}<7>%{endif}"
										  "%{if-category}%{category}: %{endif}"
										  "%{message}"));
#endif
	} else {
		qSetMessagePattern(QStringLiteral("[%{time} %{type}]\t"
										  "%{if-category}%{category}: %{endif}"
										  "%{message}"));
	}

	QCtrlSignalHandler::instance()->setAutoQuitActive(true);
	connect(QCtrlSignalHandler::instance(), &QCtrlSignalHandler::ctrlSignal,
			this, &RemindMeDaemon::signalTriggered);
	QCtrlSignalHandler::instance()->registerForSignal(SIGHUP);

	//load translations
	RemindMe::prepareTranslations(QStringLiteral("remindmed"));

	try {
		QtDataSync::Setup setup;
		RemindMe::setup(setup);
		setup.create();

		_notManager = QtMvvm::ServiceRegistry::instance()->constructInjected<NotificationManager>(this);
		_notManager->init();

		qInfo() << "daemon successfully started";
		return true;
	} catch(QException &e) {
		qCritical() << e.what();
		return false;
	}
}

void RemindMeDaemon::signalTriggered(int sig)
{
	switch (sig) {
	case SIGHUP:
		_notManager->triggerSync();
		break;
	default:
		break;
	}
}

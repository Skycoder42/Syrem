#include "syremdaemon.h"
#include <QCommandLineParser>
#include <QtMvvmCore/ServiceRegistry>
#include <QCtrlSignals>
#include <libsyrem.h>

#include "notificationmanager.h"

#include <QtCore/private/qtcore-config_p.h>

using namespace QtDataSync;

SyremDaemon::SyremDaemon(QObject *parent) :
	CoreApp{parent}
{}

int SyremDaemon::startApp(const QStringList &arguments)
{
	QCommandLineParser parser;
	parser.setApplicationDescription(QStringLiteral("The notification scheduler service for the Syrem application"));

	parser.addHelpOption();
	parser.addVersionOption();
	parser.addOption({
						 QStringLiteral("systemd-log"),
						 QStringLiteral("Log in a format that systemd can easily interpret without redundant information")
					 });

	if(!parser.parse(arguments)) {
		qCritical().noquote() << parser.errorText();
		return EXIT_FAILURE;
	}
	if(parser.isSet(QStringLiteral("help"))) {
		qInfo().noquote() << parser.helpText();
		return -1;
	}


	if(parser.isSet(QStringLiteral("systemd-log"))) {
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
			this, &SyremDaemon::signalTriggered);
#ifdef Q_OS_UNIX
	QCtrlSignalHandler::instance()->registerForSignal(SIGHUP);
#endif

	//load translations
	Syrem::prepareTranslations(QStringLiteral("syremd"));

	try {
		QtDataSync::Setup setup;
		Syrem::setup(setup);
		setup.create();

		_notManager = QtMvvm::ServiceRegistry::instance()->constructInjected<NotificationManager>(this);

		qInfo() << "daemon successfully started";
		return EXIT_SUCCESS;
	} catch(QException &e) {
		qCritical() << e.what();
		return EXIT_FAILURE;
	}
}

void SyremDaemon::signalTriggered(int sig)
{
	switch (sig) {
#ifdef Q_OS_UNIX
	case SIGHUP:
		_notManager->triggerSync();
		break;
#endif
	default:
		break;
	}
}

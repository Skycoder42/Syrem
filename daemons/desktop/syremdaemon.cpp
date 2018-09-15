#include "syremdaemon.h"
#include <QCommandLineParser>
#include <QtMvvmCore/ServiceRegistry>
#include <libsyrem.h>
#include <localsettings.h>
#include <QCtrlSignals>
#ifdef USE_SYSTEMD_SERVICE
#include <QtService/ServiceControl>
#else
#include <qautostart.h>
#include <QDir>
#endif

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

		// check if in autostart
#ifdef USE_SYSTEMD_SERVICE
		auto control = QtService::ServiceControl::create(QStringLiteral("systemd"), QStringLiteral(APPID), this);
		if(control && control->serviceExists()) {
			control->setBlocking(true);
#ifdef QT_NO_DEBUG
#else
			qDebug() << "Autostart is currently:" << (control->isAutostartEnabled() ? "enabled" : "disabled");
			if(!control->isAutostartEnabled()) {
				auto localSettings = LocalSettings::instance();
				if(!control->enableAutostart()) {
					if(!localSettings->service.autoStartChecked) {
						localSettings->service.autoStartChecked = true;
						QtMvvm::warning(tr("Autostart disabled"),
										tr("Failed to enabled autostart of the syrem service! Please enable it yourself"));
					}
				} else
					localSettings->service.autoStartChecked = false;
			}
#endif
		} else
			qWarning() << "Systemd-Unit " APPID ".service for the Syrem-Service does not exist!";
#else
		QAutoStart autoStart;
		autoStart.setStartId(QStringLiteral(APPID "d"));
		autoStart.setExtraProperty(QAutoStart::DisplayName, tr("Syrem-Service"));
		autoStart.setExtraProperty(QAutoStart::Comment, tr("The Background-Service for Syrem"));
		autoStart.setExtraProperty(QAutoStart::IconName, QStringLiteral(APPID));
		autoStart.setExtraProperty(QAutoStart::Interactive, true);
#ifdef FLATPAK_BUILD
		autoStart.setExtraProperty(QAutoStart::CustomLocation, QDir::home().absoluteFilePath(QStringLiteral(".config/autostart")));
		autoStart.setProgram(QStringLiteral("flatpak"));
		autoStart.setArguments({
								   QStringLiteral("run"),
								   QStringLiteral("--branch=master"),
#ifdef Q_PROCESSOR_X86_64
								   QStringLiteral("--arch=x86_64"),
#elif defined(Q_PROCESSOR_X86)
								   QStringLiteral("--arch=i386"),
#endif
								   QStringLiteral("--command=" PROJECT_TARGET "d"),
								   QStringLiteral(APPID)
							   });
#endif
#ifdef QT_NO_DEBUG
		if(!autoStart.isAutoStartEnabled()) {
			auto localSettings = LocalSettings::instance();
			if(!autoStart.setAutoStartEnabled()) {
				if(!localSettings->service.autoStartChecked) {
					localSettings->service.autoStartChecked = true;
					QtMvvm::warning(tr("Autostart disabled"),
									tr("Failed to enabled autostart of the syrem service! Please enable it yourself"));
				}
			} else
				localSettings->service.autoStartChecked = false;
		}
#else
		qDebug() << "Autostart is currently:" << (autoStart.isAutoStartEnabled() ? "enabled" : "disabled");
#endif
#endif

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

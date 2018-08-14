#include <QApplication>
#include <QIcon>
#include <QCommandLineParser>
#include <QtMvvmCore/ServiceRegistry>
#ifdef USE_KDE_NOTIFIER
#include "kdenotifier.h"
#else
#include "widgetsnotifier.h"
#endif

#include "remindmedaemon.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setApplicationName(QStringLiteral(PROJECT_TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QGuiApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
	QGuiApplication::setQuitOnLastWindowClosed(false);

	QtMvvm::registerInterfaceConverter<INotifier>();

#ifdef USE_KDE_NOTIFIER
	QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, KdeNotifier>();
#else
	QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, WidgetsNotifier>();
#endif

	QCommandLineParser parser;
	parser.setApplicationDescription(QStringLiteral("The notification scheduler service for the Remind-Me application"));

	parser.addHelpOption();
	parser.addVersionOption();
	parser.addOption({
						 QStringLiteral("systemd-log"),
						 QStringLiteral("Log in a format that systemd can easily interpret without redundant information")
					 });
	parser.process(a);

	RemindMeDaemon daemon;
	if(!daemon.startDaemon(parser.isSet(QStringLiteral("systemd-log"))))
		return EXIT_FAILURE;

	return a.exec();
}

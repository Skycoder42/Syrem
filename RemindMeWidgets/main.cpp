#include <QApplication>
#include <widgetpresenter.h>
#include <remindmeapp.h>
#include <remindmedaemon.h>
#include <qsingleinstance.h>
#include <QProcess>
#include <QThread>

#include "mainwindow.h"

//register the core app to be used
REGISTER_CORE_APP(RemindMeApp)

int main(int argc, char *argv[])
{
	CoreApp::disableBoot();
	QApplication a(argc, argv);
	QApplication::setApplicationName(QStringLiteral(TARGET));
	QApplication::setApplicationVersion(QStringLiteral(VERSION));
	QApplication::setOrganizationName(QStringLiteral(COMPANY));
	QApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));
	//DEBUG QApplication::setQuitOnLastWindowClosed(false);

	//NOTE: for widgets, both daemon and app are run as "one"

	QSingleInstance instance;
	instance.setStartupFunction([&](){
		//daemon
		auto daemon = new RemindMeDaemon(qApp);
		daemon->startDaemon();
		QObject::connect(&instance, &QSingleInstance::instanceMessage,
						 daemon, &RemindMeDaemon::commandMessage);


		//app
		WidgetPresenter::registerWidget<MainWindow>();

		coreApp->bootApp();
		QObject::connect(&instance, &QSingleInstance::instanceMessage,
						 coreApp, &RemindMeApp::commandMessage);

		return EXIT_SUCCESS;
	});

	return instance.singleExec();
}

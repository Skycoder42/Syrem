#include <QApplication>
#include <widgetpresenter.h>
#include <remindmeapp.h>
#include <qsingleinstance.h>
#include <QProcess>
#include <QThread>

#include "mainwindow.h"

//register the core app to be used
REGISTER_CORE_APP(RemindMeApp)

static bool ensureDaemonRunning();

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

	auto parser = coreApp->getParser();
	auto isDaemon = parser->isSet(QStringLiteral("daemon"));

	if(isDaemon)
		QApplication::setApplicationName(QApplication::applicationName() + QStringLiteral("-daemon"));

	QSingleInstance instance;
	instance.setStartupFunction([&](){
		if(!isDaemon){
			if(!ensureDaemonRunning())
				return EXIT_FAILURE;
		}

		WidgetPresenter::registerWidget<MainWindow>();
		coreApp->bootApp();

		QObject::connect(&instance, &QSingleInstance::instanceMessage,
						 coreApp, &RemindMeApp::commandMessage);

		return EXIT_SUCCESS;
	});

	return instance.singleExec();
}

static bool ensureDaemonRunning()
{
	if(!QProcess::startDetached(QCoreApplication::applicationFilePath(), {QStringLiteral("--daemon")}))
		return false;

#ifndef QT_NO_DEBUG
	QObject::connect(qApp, &QApplication::aboutToQuit, []() {
		QProcess::startDetached(QCoreApplication::applicationFilePath(), {
									QStringLiteral("--daemon"),
									QStringLiteral("--quit")
								});
	});
#endif

	return true;
}

#include <QApplication>
#include <widgetpresenter.h>
#include <remindmeapp.h>
#include <remindmedaemon.h>
#include <qsingleinstance.h>
#include <settingsdialog.h>
#include <registry.h>
#include <qhotkey.h>

#include "createreminderdialog.h"
#include "mainwindow.h"
#include "widgetsscheduler.h"
#ifdef USE_KDE_NOTIFIER
#include "kdenotifier.h"
#else
#include "widgetsnotifier.h"
#endif

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
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
	//DEBUG QApplication::setQuitOnLastWindowClosed(false);

	//NOTE: for widgets, both daemon and app are run as "one"

	QSingleInstance instance;
	instance.setStartupFunction([&](){
		//setup interfaces
		Registry::registerClass<IScheduler, WidgetsScheduler>();
#ifdef USE_KDE_NOTIFIER
		Registry::registerClass<INotifier, KdeNotifier>();
#else
		Registry::registerClass<INotifier, WidgetsNotifier>();
#endif

		//daemon
		auto daemon = new RemindMeDaemon(qApp);
		daemon->startDaemon();
		QObject::connect(&instance, &QSingleInstance::instanceMessage,
						 daemon, &RemindMeDaemon::commandMessage);

		//app
		WidgetPresenter::registerWidget<MainWindow>();
		WidgetPresenter::registerWidget<CreateReminderDialog>();
		WidgetPresenter::registerWidget<SettingsDialog>();

		coreApp->bootApp();
		QObject::connect(&instance, &QSingleInstance::instanceMessage,
						 coreApp, &RemindMeApp::commandMessage);

		//hotkey
		auto hk = new QHotkey(qApp);
		QObject::connect(hk, &QHotkey::activated, coreApp, [](){
			auto control = new CreateReminderControl();
			control->setDeleteOnClose(true);
			control->show();
		});
		hk->setShortcut(QKeySequence(QSettings().value(QStringLiteral("gui/hotkey"), QStringLiteral("CTRL+META+R")).toString()), true);//TODO settings

		return EXIT_SUCCESS;
	});

	return instance.singleExec();
}

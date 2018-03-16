#include <QApplication>
#include <QKeySequenceEdit>
#include <QTimeEdit>
#include <QtDataSync/MigrationHelper>
#include <QtMvvmCore/ServiceRegistry>
#include <QtMvvmWidgets/WidgetsPresenter>
#include <QtMvvmWidgets/SettingsDialog>
#include <QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h>
#include <remindmeapp.h>
#include <remindmedaemon.h>
#include <qsingleinstance.h>
#include <qhotkey.h>

#include "createreminderdialog.h"
#include "mainwindow.h"
#include "widgetsscheduler.h"
#include "snoozetimesedit.h"
#include "snoozedialog.h"

#ifdef USE_KDE_NOTIFIER
#include "kdenotifier.h"
#else
#include "widgetsnotifier.h"
#endif

//register the core app to be used
QTMVVM_REGISTER_CORE_APP(RemindMeApp)

int main(int argc, char *argv[])
{
	QtMvvm::CoreApp::disableAutoBoot();
	QApplication a(argc, argv);
	QApplication::setApplicationName(QStringLiteral(TARGET));
	QApplication::setApplicationVersion(QStringLiteral(VERSION));
	QApplication::setOrganizationName(QStringLiteral(COMPANY));
	QApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
	QApplication::setQuitOnLastWindowClosed(false);

	//NOTE: for widgets, both daemon and app are run as "one"

	QSingleInstance instance;
	instance.setStartupFunction([&](){
		//setup interfaces
		QtMvvm::ServiceRegistry::instance()->registerInterface<IScheduler, WidgetsScheduler>();
#ifdef USE_KDE_NOTIFIER
		QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, KdeNotifier>();
#else
		QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, WidgetsNotifier>();
#endif

		//daemon
		auto daemon = new RemindMeDaemon(qApp);
		daemon->startDaemon();
		QObject::connect(&instance, &QSingleInstance::instanceMessage,
						 daemon, &RemindMeDaemon::commandMessage);

		//app
		QtMvvm::registerDataSyncWidgets();
		QtMvvm::WidgetsPresenter::registerView<MainWindow>();
		QtMvvm::WidgetsPresenter::registerView<CreateReminderDialog>();
		QtMvvm::WidgetsPresenter::registerView<SnoozeDialog>();
		auto wPres = dynamic_cast<QtMvvm::WidgetsPresenter*>(QtMvvm::ServiceRegistry::instance()->service<QtMvvm::IPresenter>());
		wPres->inputWidgetFactory()->addSimpleWidget<SnoozeTimes, SnoozeTimesEdit>();

		coreApp->bootApp();
		QObject::connect(&instance, &QSingleInstance::instanceMessage,
						 coreApp, &RemindMeApp::commandMessage);

		//hotkey
		auto hk = new QHotkey(qApp);
		QObject::connect(hk, &QHotkey::activated, coreApp, [](){
			coreApp->show<CreateReminderViewModel>();
		});
		hk->setShortcut(QKeySequence(QSettings().value(QStringLiteral("gui/hotkey"), QStringLiteral("CTRL+META+R")).toString()), true);

//		//migration helper
//		QtDataSync::MigrationHelper helper;
//		helper.startMigration(QtDataSync::MigrationHelper::DefaultOldStorageDir, QtDataSync::MigrationHelper::MigrateData);

		return EXIT_SUCCESS;
	});

	return instance.singleExec();
}

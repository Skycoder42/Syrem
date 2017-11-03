#include <QApplication>
#include <QKeySequenceEdit>
#include <widgetpresenter.h>
#include <remindmeapp.h>
#include <remindmedaemon.h>
#include <qsingleinstance.h>
#include <settingsdialog.h>
#include <registry.h>
#include <qhotkey.h>
#include <QTimeEdit>
#include <QtDataSync/UserDataNetworkExchange>

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
		WidgetPresenter::registerWidget<SnoozeDialog>();
		WidgetPresenter::registerWidget<SettingsDialog>();
		WidgetPresenter::inputWidgetFactory()->addSimpleWidget<QKeySequence, QKeySequenceEdit>();
		WidgetPresenter::inputWidgetFactory()->addSimpleWidget<QTime, QTimeEdit>();
		WidgetPresenter::inputWidgetFactory()->addSimpleWidget<SnoozeTimes, SnoozeTimesEdit>();

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
		hk->setShortcut(QKeySequence(QSettings().value(QStringLiteral("gui/hotkey"), QStringLiteral("CTRL+META+R")).toString()), true);

		//DEBUG ID EXCHANGE until fixed upstream
		auto settings = new QSettings(qApp);
		if(!settings->value(QStringLiteral("hasId")).toBool()) {
			auto exchange = new QtDataSync::UserDataNetworkExchange(qApp);
			exchange->setDeviceName(QStringLiteral("remindme.widgets"));
			QObject::connect(exchange, &QtDataSync::UserDataNetworkExchange::usersChanged, [exchange, settings](QList<QtDataSync::UserInfo> infos) {
				foreach(auto info, infos) {
					if(info.name() == QStringLiteral("remindme.quick")) {
						exchange->exportTo(info, QStringLiteral("baum42"));
						exchange->deleteLater();
						settings->setValue(QStringLiteral("hasId"), true);
						settings->deleteLater();
					}
				}
			});
		} else
			settings->deleteLater();

		return EXIT_SUCCESS;
	});

	return instance.singleExec();
}

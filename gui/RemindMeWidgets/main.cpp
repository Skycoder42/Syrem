#include <QApplication>
#include <QtMvvmWidgets/WidgetsPresenter>
#include <QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h>
#include <remindmeapp.h>

#include "mainwindow.h"
#include "createreminderdialog.h"
#include "snoozedialog.h"
#include "snoozetimesedit.h"

// Register the core app
QTMVVM_REGISTER_CORE_APP(RemindMeApp)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QtMvvm::registerDataSyncWidgets();
	QtMvvm::WidgetsPresenter::registerView<MainWindow>();
	QtMvvm::WidgetsPresenter::registerView<CreateReminderDialog>();
	QtMvvm::WidgetsPresenter::registerView<SnoozeDialog>();
	auto wPres = dynamic_cast<QtMvvm::WidgetsPresenter*>(QtMvvm::ServiceRegistry::instance()->service<QtMvvm::IPresenter>());
	wPres->inputWidgetFactory()->addSimpleWidget<SnoozeTimes, SnoozeTimesEdit>();

//		//migration helper
//		QtDataSync::MigrationHelper helper;
//		helper.startMigration(QtDataSync::MigrationHelper::DefaultOldStorageDir, QtDataSync::MigrationHelper::MigrateData);

	return a.exec();
}

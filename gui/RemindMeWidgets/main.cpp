#include <QApplication>
#include <QtMvvmWidgets/WidgetsPresenter>
#include <QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h>
#include <remindmeapp.h>

#include "mainwindow.h"
#include "createreminderdialog.h"

// Register the core app
QTMVVM_REGISTER_CORE_APP(RemindMeApp)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QtMvvm::registerDataSyncWidgets();
	QtMvvm::WidgetsPresenter::registerView<MainWindow>();
	QtMvvm::WidgetsPresenter::registerView<CreateReminderDialog>();

	return a.exec();
}

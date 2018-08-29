#include <QApplication>
#include <QIcon>
#include <QCommandLineParser>
#include <QtMvvmCore/ServiceRegistry>
#include <QtMvvmWidgets/WidgetsPresenter>
#ifdef USE_KDE_NOTIFIER
#include "kdenotifier.h"
#else
#include "widgetsnotifier.h"
#endif

#include "remindmedaemon.h"
#include "../../gui/RemindMeWidgets/snoozedialog.h"
#include "../../gui/RemindMeWidgets/termselectiondialog.h"

// Register the core app
QTMVVM_REGISTER_CORE_APP(RemindMeDaemon)

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
	QtMvvm::WidgetsPresenter::registerView<SnoozeDialog>();
	QtMvvm::WidgetsPresenter::registerView<TermSelectionDialog>();

#ifdef USE_KDE_NOTIFIER
	QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, KdeNotifier>();
#else
	QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, WidgetsNotifier>();
#endif

	return a.exec();
}

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

#include "syremdaemon.h"
#include "../../gui/widgets/snoozedialog.h"
#include "../../gui/widgets/termselectiondialog.h"
#include "traysnoozedialog.h"

// Register the core app
QTMVVM_REGISTER_CORE_APP(SyremDaemon)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setApplicationName(QStringLiteral(PROJECT_TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QGuiApplication::setApplicationDisplayName(QStringLiteral(PROJECT_NAME));
	QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));
	QGuiApplication::setQuitOnLastWindowClosed(false);

	QtMvvm::registerInterfaceConverter<INotifier>();
	QtMvvm::WidgetsPresenter::registerView<SnoozeDialog>();
	QtMvvm::WidgetsPresenter::registerView<TermSelectionDialog>();
	QtMvvm::WidgetsPresenter::registerView<TraySnoozeDialog>();

#ifdef USE_KDE_NOTIFIER
	QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, KdeNotifier>();
#else
	QtMvvm::ServiceRegistry::instance()->registerInterface<INotifier, WidgetsNotifier>();
#endif

	return a.exec();
}

#include <QApplication>
#include <QIcon>
#include <QDebug>
#include <remindmelib.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setApplicationName(QStringLiteral("Remind-Me"));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QGuiApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
	QGuiApplication::setQuitOnLastWindowClosed(false);

	try {
		QtDataSync::Setup setup;
		RemindMe::setup(setup);
		setup.create();
		qInfo() << "daemon started";

		return a.exec();
	} catch(QException &e) {
		qCritical() << e.what();
		return EXIT_FAILURE;
	}
}

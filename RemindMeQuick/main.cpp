#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <quickpresenter.h>
#include <remindmeapp.h>
#include <remindmedaemon.h>

REGISTER_CORE_APP(RemindMeApp)

static void setupApp();
static void setupDaemon();

int main(int argc, char *argv[])
{
	CoreApp::disableBoot();
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);
	QGuiApplication::setApplicationName(QStringLiteral(TARGET));
	QGuiApplication::setApplicationVersion(QStringLiteral(VERSION));
	QGuiApplication::setOrganizationName(QStringLiteral(COMPANY));
	QGuiApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QGuiApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));

	auto parser = coreApp->getParser();
	if(parser->isSet(QStringLiteral("daemon")))
		setupDaemon();
	else
		setupApp();

	return app.exec();
}

static void setupApp()
{
	qmlRegisterUncreatableType<MainControl>("de.skycoder42.remindme", 1, 0, "MainControl", QStringLiteral("Controls cannot be created!"));

	QuickPresenter::createAppEngine(QUrl(QLatin1String("qrc:/qml/App.qml")));

	QMetaObject::invokeMethod(coreApp, "bootApp", Qt::QueuedConnection);
}

static void setupDaemon()
{
	auto daemon = new RemindMeDaemon(qApp);
	daemon->startDaemon();
}

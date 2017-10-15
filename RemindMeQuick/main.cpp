#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <quickpresenter.h>
#include <remindmeapp.h>

REGISTER_CORE_APP(RemindMeApp)

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	qmlRegisterUncreatableType<MainControl>("com.example.remindme", 1, 0, "MainControl", QStringLiteral("Controls cannot be created!"));

	QuickPresenter::createAppEngine(QUrl(QLatin1String("qrc:/qml/App.qml")));

	return app.exec();
}

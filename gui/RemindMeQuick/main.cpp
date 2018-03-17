#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtMvvmDataSyncQuick/qtmvvmdatasyncquick_global.h>
#include <remindmeapp.h>
#include <mainviewmodel.h>
#include <createreminderviewmodel.h>
#include <snoozeviewmodel.h>

QTMVVM_REGISTER_CORE_APP(RemindMeApp)

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	qmlRegisterUncreatableType<Reminder>("de.skycoder42.remindme", 1, 0, "Reminder", QStringLiteral("Q_GADGETs cannot be created!"));
	qmlRegisterUncreatableType<MainViewModel>("de.skycoder42.remindme", 1, 0, "MainViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<CreateReminderViewModel>("de.skycoder42.remindme", 1, 0, "CreateReminderViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<SnoozeViewModel>("de.skycoder42.remindme", 1, 0, "SnoozeViewModel", QStringLiteral("ViewModels cannot be created!"));

	QtMvvm::registerDataSyncQuick();

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/qml/App.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

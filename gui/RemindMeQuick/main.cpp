#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtMvvmQuick/QuickPresenter>
#include <QtMvvmDataSyncQuick/qtmvvmdatasyncquick_global.h>
#include <remindmeapp.h>
#include <mainviewmodel.h>
#include <createreminderviewmodel.h>
#include <snoozeviewmodel.h>
#include <snoozetimes.h>

QTMVVM_REGISTER_CORE_APP(RemindMeApp)

namespace {

QObject *create_snooze_times_generator(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
	Q_UNUSED(jsEngine)
	return new SnoozeTimesGenerator(qmlEngine);
}

}

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	qmlRegisterSingletonType<SnoozeTimesGenerator>("de.skycoder42.remindme", 1, 0, "SnoozeTimes", create_snooze_times_generator);
	qmlRegisterUncreatableType<Reminder>("de.skycoder42.remindme", 1, 0, "Reminder", QStringLiteral("Q_GADGETs cannot be created!"));
	qmlRegisterUncreatableType<MainViewModel>("de.skycoder42.remindme", 1, 0, "MainViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<CreateReminderViewModel>("de.skycoder42.remindme", 1, 0, "CreateReminderViewModel", QStringLiteral("ViewModels cannot be created!"));
	qmlRegisterUncreatableType<SnoozeViewModel>("de.skycoder42.remindme", 1, 0, "SnoozeViewModel", QStringLiteral("ViewModels cannot be created!"));

	QtMvvm::registerDataSyncQuick();
	auto qPres = dynamic_cast<QtMvvm::QuickPresenter*>(QtMvvm::ServiceRegistry::instance()->service<QtMvvm::IPresenter>());
	qPres->inputViewFactory()->addSimpleInput<QTime>(QStringLiteral("qrc:/qtmvvm/inputs/TimeEdit.qml"));
	qPres->inputViewFactory()->addSimpleInput<SnoozeTimes>(QStringLiteral("qrc:/qtmvvm/inputs/SnoozeTimesEdit.qml"));

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/qml/App.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

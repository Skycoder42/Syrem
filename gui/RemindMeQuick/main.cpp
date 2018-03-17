#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <remindmeapp.h>
#include <mainviewmodel.h>

QTMVVM_REGISTER_CORE_APP(RemindMeApp)

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	// If you want to support file dialogs on platforms other then android, use a QApplication instead (and add QT += widgets to the pro file)
	QGuiApplication app(argc, argv);

	qmlRegisterUncreatableType<MainViewModel>("com.example.remindme", 1, 0, "MainViewModel", QStringLiteral("ViewModels cannot be created!"));

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/App.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

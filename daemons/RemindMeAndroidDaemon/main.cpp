#include <QCoreApplication>
#include <QtDataSync>
#include <remindmelib.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	//workarond
	qputenv("PLUGIN_KEYSTORES_PATH", QCoreApplication::applicationDirPath().toUtf8());
	qInfo() << "Overwriting keystore path to:" << qgetenv("PLUGIN_KEYSTORES_PATH");

	try {
		QtDataSync::Setup setup;
		RemindMe::setup(setup);
		setup.create();
		qInfo() << "daemon successfully started";
	} catch(QException &e) {
		qCritical() << e.what();
		return EXIT_FAILURE;
	}

	return a.exec();
}

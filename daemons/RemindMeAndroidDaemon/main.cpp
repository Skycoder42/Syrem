#include <QCoreApplication>
#include "remindmeservice.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	//workarond
	qputenv("PLUGIN_KEYSTORES_PATH", QCoreApplication::applicationDirPath().toUtf8());
	qInfo() << "Overwriting keystore path to:" << qgetenv("PLUGIN_KEYSTORES_PATH");

	RemindmeService service;
	if(!service.startService())
		return EXIT_FAILURE;

	return a.exec();
}

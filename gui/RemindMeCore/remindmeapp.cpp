#include "remindmeapp.h"
#include "mainviewmodel.h"

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QIcon>

RemindMeApp::RemindMeApp(QObject *parent) :
	CoreApp(parent)
{
	QCoreApplication::setApplicationName(QStringLiteral("Remind-Me"));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QGuiApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
}

void RemindMeApp::performRegistrations()
{
	//if you are using a qt resource (e.g. "remindmecore.qrc"), initialize it here
	Q_INIT_RESOURCE(remindmecore);
}

int RemindMeApp::startApp(const QStringList &arguments)
{
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.addHelpOption();

	//add more options

	//shows help or version automatically
	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;

	//show a viewmodel to complete the startup
	show<MainViewModel>();
	return EXIT_SUCCESS;
}

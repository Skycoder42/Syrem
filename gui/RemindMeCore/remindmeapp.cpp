#include "remindmeapp.h"

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QIcon>
#include <QLibraryInfo>
#include <QTranslator>

#include <remindmelib.h>

#include "mainviewmodel.h"

RemindMeApp::RemindMeApp(QObject *parent) :
	CoreApp(parent),
	_daemon(nullptr)
{
	QCoreApplication::setApplicationName(QStringLiteral("remind-me"));
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

	//load translations
	RemindMe::prepareTranslations(QStringLiteral("remind-me"));
}

int RemindMeApp::startApp(const QStringList &arguments)
{
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.addHelpOption();

	//add more options
	parser.addOption({
						 {QStringLiteral("s"), QStringLiteral("select")},
						 tr("Select a <reminder> from the list by its id"),
						 tr("reminder")
					 });

	//shows help or version automatically
	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;

	// start the daemon
	_daemon = new DaemonController(this);
	_daemon->ensureStarted();
	//DEBUG
	connect(qApp, &QGuiApplication::aboutToQuit,
			_daemon, &DaemonController::stop);

	// create datasync etc
	QtDataSync::Setup setup;
	RemindMe::setup(setup);
	auto warn = !setup.createPassive(QtDataSync::DefaultSetup, 3000);

	//show a viewmodel to complete the startup
	if(parser.isSet(QStringLiteral("select")))
		show<MainViewModel>(MainViewModel::showParams(QUuid(parser.value(QStringLiteral("select")))));
	else
		show<MainViewModel>();
	if(warn) {
		QtMvvm::warning(tr("Service unavailable"),
						tr("Failed to connect to service. The application will still work, "
						   "but neither synchronize nor schedule new reminders!"));
	}

	return EXIT_SUCCESS;
}

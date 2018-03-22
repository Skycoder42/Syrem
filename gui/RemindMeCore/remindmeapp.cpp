#include "remindmeapp.h"

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QIcon>
#include <QLibraryInfo>
#include <QTranslator>
#include <QDebug>
#include <QtMvvmCore/ServiceRegistry>

#include <remindmelib.h>

#include "mainviewmodel.h"
#include "createreminderviewmodel.h"

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
	RemindMe::prepareTranslations(QStringLiteral("remindme"));
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
	parser.addOption({
						 {QStringLiteral("c"), QStringLiteral("create")},
						 tr("Show the create reminder dialog instead of the mainwindow. "
							"Can be used to create a reminder from a shortcut.")
					 });
	parser.addOption({
						 {QStringLiteral("a"), QStringLiteral("add")},
						 tr("Programatically add a reminder without showing a GUI. Use as:\n"
							"remind-me --add [--important] <description> <when>")
					 });
	parser.addOption({
						 {QStringLiteral("i"), QStringLiteral("important")},
						 tr("Use in combination with \"--add\" to create an important reminder.")
					 });

	//shows help or version automatically
	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;

	// start the daemon
	_daemon = new DaemonController(this);
	_daemon->ensureStarted();
#ifndef QT_NO_DEBUG
	connect(qApp, &QGuiApplication::aboutToQuit,
			_daemon, &DaemonController::stop);
#endif

	// create datasync etc
	QtDataSync::Setup setup;
	RemindMe::setup(setup);
	auto warn = !setup.createPassive(QtDataSync::DefaultSetup, 3000);

	//show a viewmodel to complete the startup
	if(parser.isSet(QStringLiteral("add"))) {
		if(parser.positionalArguments().size() != 2) {
			qCritical().noquote() << tr("Invalid arguments for add option. Use as: remind-me --add [--important] <description> <when>");
			return EXIT_FAILURE;
		}

		if(warn) {
			qWarning().noquote() << tr("Failed to connect to service. The application will still work, "
									   "but neither synchronize nor schedule new reminders!");
			warn = false;
		}

		QMetaObject::invokeMethod(this, "createReminderInline", Qt::QueuedConnection,
								  Q_ARG(bool, parser.isSet(QStringLiteral("important"))),
								  Q_ARG(QString, parser.positionalArguments()[0]),
								  Q_ARG(QString, parser.positionalArguments()[1]));
	} else if(parser.isSet(QStringLiteral("create")))
		show<CreateReminderViewModel>();
	else if(parser.isSet(QStringLiteral("select")))
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

void RemindMeApp::createReminderInline(bool important, const QString &description, const QString &when)
{
	try {
		DateParser* parser = QtMvvm::ServiceRegistry::instance()->service<DateParser>();
		ReminderStore store;

		Reminder reminder;
		reminder.setImportant(important);
		reminder.setDescription(description);
		reminder.setSchedule(parser->parseSchedule(when));

		store.save(reminder);
		qInfo().noquote() << tr("Successfully created reminder. Next trigger at:")
						  << reminder.current().toString(Qt::DefaultLocaleLongDate);

		QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
		return;
	} catch(DateParserException &e) {
		qCritical().noquote() << tr("Invalid <when> date:") << e.what();
	} catch(QException &e) {
		qCritical().noquote() << tr("Failed to save reminder. Original error:")
							  << e.what();
	}

	qApp->exit(EXIT_FAILURE);
}

#include "remindmeapp.h"

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QIcon>
#include <QLibraryInfo>
#include <QTranslator>
#include <QDebug>
#include <QtMvvmCore/ServiceRegistry>
#include <QTimer>
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif

#include <remindmelib.h>
#include <eventexpressionparser.h>

#include "mainviewmodel.h"
#include "createreminderviewmodel.h"

RemindMeApp::RemindMeApp(QObject *parent) :
	CoreApp{parent}
{
	QCoreApplication::setApplicationName(QStringLiteral(PROJECT_TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QGuiApplication::setApplicationDisplayName(QStringLiteral(DISPLAY_NAME));
	QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.svg")));
	QGuiApplication::setQuitOnLastWindowClosed(false);
}

bool RemindMeApp::isCreateOnly() const
{
	return _createOnly;
}

void RemindMeApp::performRegistrations()
{
	Q_INIT_RESOURCE(remindmecore);
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


#ifndef Q_OS_ANDROID
#if !defined(QT_NO_DEBUG)
	_serviceControl = QtService::ServiceControl::create(QStringLiteral("standard"),
														QCoreApplication::applicationDirPath() +
														QStringLiteral("/../../daemons/RemindMeDesktopDaemon/remind-med"),
														this);
#elif defined(Q_OS_LINUX)
	_serviceControl = QtService::ServiceControl::create(QStringLiteral("systemd"),
														QStringLiteral(PROJECT_TARGET),
														this);
#else
	_serviceControl = QtService::ServiceControl::create(QStringLiteral("standard"),
														QCoreApplication::applicationDirPath() + QStringLiteral("/remind-med"),
														this);
#endif
	if(!_serviceControl->start())
		qWarning() << "Failed to start service";
#ifndef QT_NO_DEBUG
	connect(qApp, &QGuiApplication::aboutToQuit,
			_serviceControl, &QtService::ServiceControl::stop);
#endif
#endif

	// create datasync etc.
	auto progress = QtMvvm::showBusy(this,
									 tr("Starting Service"),
									 tr("Starting the Remind-Me service. Please wait until it started…"),
									 false);
	QtDataSync::Setup setup;
	RemindMe::setup(setup);
	auto warn = !setup.createPassive(QtDataSync::DefaultSetup, 5000);
	progress->close();

#ifdef Q_OS_ANDROID
	bool forceCreate = QtAndroid::androidActivity().callMethod<jboolean>("isCreateOnly");
#else
	auto forceCreate = false;
#endif

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

		createReminderInline(parser.isSet(QStringLiteral("important")),
							 parser.positionalArguments()[0],
							 parser.positionalArguments()[1]);
	} else if(parser.isSet(QStringLiteral("create")) || forceCreate) {
		_createOnly = true;
		show<CreateReminderViewModel>();
	} else if(parser.isSet(QStringLiteral("select")))
		show<MainViewModel>(MainViewModel::showParams(QUuid{parser.value(QStringLiteral("select"))}));
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
		auto parser = QtMvvm::ServiceRegistry::instance()->service<EventExpressionParser>();
		auto store = new ReminderStore{this};

		auto terms = parser->parseMultiExpression(when);
		if(parser->needsSelection(terms)) {
			for(const auto &term : qAsConst(terms)) {
				if(parser->needsSelection(term)) {
					qCritical().noquote() << "Expression has more than one interpretation. This is currently not supported for the command line API.\n"
										  << "Possible interpretations are:";
					for(const auto &sTerm : term)
						qCritical().noquote() << "\t- " << sTerm.describe();
				}
			}

			qApp->exit(EXIT_FAILURE);
			return;
		}

		Reminder reminder;
		reminder.setImportant(important);
		reminder.setDescription(description);
		reminder.setSchedule(parser->createMultiSchedule(terms));

		connect(store, &QtDataSync::DataTypeStoreBase::dataChanged,
				this, [reminder](const QString &id){
			if(id == reminder.id().toString())
				qApp->quit();
		});

		store->save(reminder);
		qInfo().noquote() << tr("Reminder-ID:") << reminder.id().toString(QUuid::WithBraces);
		qInfo().noquote() << tr("Successfully created reminder. Next trigger at:")
						  << reminder.current().toString(Qt::DefaultLocaleLongDate);

		QTimer::singleShot(5000, qApp, &QCoreApplication::quit);
		return;
	} catch(EventExpressionParserException &e) {
		qCritical().noquote() << tr("Invalid <when> date:") << e.what();
	} catch(QException &e) {
		qCritical().noquote() << tr("Failed to save reminder. Original error:")
							  << e.what();
	}

	qApp->exit(EXIT_FAILURE);
}

﻿#include "syremapp.h"

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

#include <libsyrem.h>
#include <eventexpressionparser.h>

#include "mainviewmodel.h"
#include "createreminderviewmodel.h"

SyremApp::SyremApp(QObject *parent) :
	CoreApp{parent}
{
	QCoreApplication::setApplicationName(QStringLiteral(PROJECT_TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE));
	QGuiApplication::setApplicationDisplayName(QStringLiteral(PROJECT_NAME));
	QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));
	QGuiApplication::setQuitOnLastWindowClosed(false);
}

bool SyremApp::isCreateOnly() const
{
	return _createOnly;
}

void SyremApp::resetIsCreateOnly()
{
	_createOnly = false;
}

void SyremApp::performRegistrations()
{
	Q_INIT_RESOURCE(syrem_core);
	Syrem::prepareTranslations(QStringLiteral("syrem"));

	registerInputTypeMapping<QVariant>("action");
}

int SyremApp::startApp(const QStringList &arguments)
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
							"syrem --add [--important] <description> <when>")
					 });
	parser.addOption({
						 {QStringLiteral("i"), QStringLiteral("important")},
						 tr("Use in combination with \"--add\" to create an important reminder.")
					 });

	//shows help or version automatically
	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;


#ifdef USE_DEBUG_SERVICE
	_serviceControl = new QProcess{this};
	_serviceControl->setProgram(QCoreApplication::applicationDirPath() +
							#ifdef Q_OS_WIN
								QStringLiteral("/../../daemons/desktop/debug/syremd")
							#else
								QStringLiteral("/../../daemons/desktop/syremd")
							#endif
								);
	_serviceControl->setProcessChannelMode(QProcess::ForwardedChannels);
	_serviceControl->start();
	connect(qApp, &QGuiApplication::aboutToQuit,
			this, [this]() {
		_serviceControl->terminate();
		_serviceControl->waitForFinished(1000);
	});
#elif defined(USE_SYSTEMD_SERVICE)
	_serviceControl = QtService::ServiceControl::create(QStringLiteral("systemd"),
														QStringLiteral(PROJECT_TARGET),
														this);
	_serviceControl->start();
#elif defined(USE_RELEASE_SERVICE)
	if(!QProcess::startDetached(QCoreApplication::applicationDirPath() + QStringLiteral("/syremd")))
		qCritical() << "Failed to start service";
#endif

	// create datasync etc.
	auto progress = QtMvvm::showBusy(this,
									 tr("Starting Service"),
									 tr("Starting the Syrem service. Please wait until it started…"),
									 false);
	QtDataSync::Setup setup;
	Syrem::setup(setup);
	auto warn = !setup.createPassive(QtDataSync::DefaultSetup, 30000);
	progress->close();

#ifdef Q_OS_ANDROID
	bool forceCreate = QtAndroid::androidActivity().callMethod<jboolean>("isCreateOnly");
#else
	auto forceCreate = false;
#endif

	//show a viewmodel to complete the startup
	if(parser.isSet(QStringLiteral("add"))) {
		if(parser.positionalArguments().size() != 2) {
			qCritical().noquote() << tr("Invalid arguments for add option. Use as: syrem --add [--important] <description> <when>");
			return EXIT_FAILURE;
		}

		if(warn) {
			qWarning().noquote() << tr("Failed to connect to service. The application will still work, "
									   "but neither synchronize nor schedule new reminders!");
			warn = false;
		}

		createReminderInline(parser.isSet(QStringLiteral("important")),
							 parser.positionalArguments().value(0),
							 parser.positionalArguments().value(1));
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

void SyremApp::createReminderInline(bool important, const QString &description, const QString &when)
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
		reminder.setExpression(when);

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

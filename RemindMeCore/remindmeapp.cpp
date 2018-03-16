#include "remindmeapp.h"
#include "rep_remindermanager_replica.h"
#include "snoozeviewmodel.h"
#include "snoozetimes.h"

#include <QLibraryInfo>
#include <QTranslator>
#include <QCommandLineParser>

RemindMeApp::RemindMeApp(QObject *parent) :
	CoreApp(parent),
	_roNode(nullptr),
	_mainControl(nullptr)
{}

QRemoteObjectNode *RemindMeApp::node() const
{
	return _roNode;
}

void RemindMeApp::setupParser(QCommandLineParser &parser) const
{
	parser.setApplicationDescription(tr("A simple reminder application for desktop and mobile, with synchronized reminders."));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addOption({
						 {QStringLiteral("d"), QStringLiteral("daemon")},
						 tr("Start the Remind-Me daemon instead of the standard GUI.")
					 });
	parser.addOption({
						 {QStringLiteral("c"), QStringLiteral("create")},
						 tr("Create a new reminder. Pass the text and when expression as arguments. "
							"IMPORTANT: Make shure the daemon is running BEFORE creating any reminders, or it won't work!")
					 });
	parser.addOption({
						 {QStringLiteral("i"), QStringLiteral("important")},
						 tr("Create an important reminder. Can only be used together with --create!")
					 });
	parser.addOption({
						 QStringLiteral("quit"),
						 tr("Quits the daemon gracefully (daemon only).")
					 });
}

void RemindMeApp::commandMessage(const QStringList &message)
{
	QCommandLineParser parser;
	setupParser(parser);
	if(!parser.parse(message))
		qWarning() << "Invalid arguments received!";
	else {
		if(parser.isSet(QStringLiteral("daemon")))
			return;

		if(parser.isSet(QStringLiteral("create"))) {
			if(parser.positionalArguments().size() < 2) {
				qWarning() << "Invalid create arguments! Must be:" << QCoreApplication::applicationName() << "--create [--important] <text> <when>";
				return;
			}

			auto text = parser.positionalArguments().value(0);
			auto when = parser.positionalArguments().value(1);
			auto important = parser.isSet(QStringLiteral("important"));
			createFromCli(text, when, important);
		} else
			showMainControl();
	}
}

void RemindMeApp::showMainControl()
{
	show<MainViewModel>(); //TODO or activate existing
}

void RemindMeApp::showSnoozeControl(const QUuid &id, quint32 versionCode)
{
	if(id.isNull())
		return;
	show<SnoozeViewModel>(SnoozeViewModel::showParams(id, versionCode));
}

void RemindMeApp::performRegistrations()
{
	SnoozeTimes::setup();

	Q_INIT_RESOURCE(remindmecore);

	//load translations
	auto translator = new QTranslator(this);
	if(translator->load(QLocale(),
						QStringLiteral("remindme"),
						QStringLiteral("_"),
						QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
		qApp->installTranslator(translator);
	else {
		qWarning() << "Failed to load translations! Switching to C-locale for a consistent experience";
		delete translator;
		QLocale::setDefault(QLocale::c());
	}
}

int RemindMeApp::startApp(const QStringList &arguments)
{
	QCommandLineParser parser;
	setupParser(parser);

	//shows help or version automatically
	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;

	_roNode = new QRemoteObjectNode(this);
	_roNode->setName(QStringLiteral("widgets-main"));
	if(!_roNode->connectToNode(QUrl(QStringLiteral("local:remindme-daemon")))) {
		qCritical() << _roNode->lastError();
		return EXIT_FAILURE;
	}
	connect(_roNode, &QRemoteObjectNode::error, this, [this](QRemoteObjectNode::ErrorCode errorCode){
		qCritical() << "RO_ERROR:" << errorCode;
	});

	if(!parser.isSet(QStringLiteral("daemon")))
		showMainControl();
	return EXIT_SUCCESS;
}

void RemindMeApp::createFromCli(const QString &text, const QString &when, bool important)
{
	auto manager = _roNode->acquire<ReminderManagerReplica>();
	if(!manager) {
		qCritical() << _roNode->lastError();
		return;
	}

	if(manager->isInitialized())
		manager->createReminder(text, important, when);
	else {
		connect(manager, &ReminderManagerReplica::initialized, this, [=](){
			manager->createReminder(text, important, when);
		});
	}

	connect(manager, &ReminderManagerReplica::reminderCreated, this, [manager](){
		qInfo() << "Successfully create reminder from CLI";
		manager->deleteLater();
	});
	connect(manager, &ReminderManagerReplica::reminderError, this, [manager](bool isCreate, const QString &error){
		if(!isCreate)
			return;
		qCritical().noquote() << "Failed to create reminder from CLI with error:" << error;
		manager->deleteLater();
	});
}

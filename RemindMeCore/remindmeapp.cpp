#include "remindermanager.h"
#include "remindmeapp.h"

#include "rep_remindermanager_replica.h"

RemindMeApp::RemindMeApp(QObject *parent) :
	CoreApp(parent),
	_roNode(nullptr),
	_mainControl(nullptr)
{
	//register metatypes etc here, just like you would do in your main before call QCoreApplication::exec

	//if you are using a qt resource (e.g. "remindmecore.qrc"), initialize it here
	//Q_INIT_RESOURCE(remindmecore);
}

void RemindMeApp::commandMessage(const QStringList &message)
{
	auto parser = getParser();
	if(!parser->parse(message))
		qWarning() << "Invalid arguments received!";
	else {
		if(parser->isSet(QStringLiteral("quit"))){
			qInfo() << "Received quit command, stopping daemon";
			qApp->quit();
		}
	}
}

void RemindMeApp::setupParser(QCommandLineParser &parser, bool &allowInvalid) const
{
	CoreApp::setupParser(parser, allowInvalid);

	parser.setApplicationDescription(tr("A simple reminder application for desktop and mobile, with synchronized reminders."));
	parser.addOption({
						 {QStringLiteral("d"), QStringLiteral("daemon")},
						 tr("Start the RemindMe daemon instead of the standard GUI.")
					 });
	parser.addOption({
						 QStringLiteral("quit"),
						 tr("Quits the daemon gracefully (daemon only).")
					 });
}

bool RemindMeApp::startApp(const QCommandLineParser &parser)
{
	//shows help or version automatically
	if(autoShowHelpOrVersion(parser))
		return true;

	if(parser.isSet(QStringLiteral("daemon")))
		return startDaemon();
	else
		return startMainGui();
}

void RemindMeApp::aboutToQuit()
{
	//if you need to perform any cleanups, do it here
}

bool RemindMeApp::startMainGui()
{
	_roNode = new QRemoteObjectNode(this);
	_roNode->setName(QStringLiteral("widgets-main"));
	if(!_roNode->connectToNode(QUrl(QStringLiteral("local:remindme")))) {
		qCritical() << _roNode->lastError();
		return false;
	}

	auto manager = _roNode->acquire<ReminderManagerReplica>();
	if(!manager) {
		qCritical() << _roNode->lastError();
		return false;
	}

	_mainControl = new MainControl(this);
	showControl(_mainControl);
	return true;
}

bool RemindMeApp::startDaemon()
{
	auto node = new QRemoteObjectHost(this);
	node->setName(QStringLiteral("daemon"));
	if(!node->setHostUrl(QUrl(QStringLiteral("local:remindme")))) {
		qCritical() << node->lastError();
		return false;
	}
	_roNode = node;

	auto rmManager = new ReminderManager(this);
	if(!node->enableRemoting(rmManager)) {
		qCritical() << node->lastError();
		return false;
	}

	qDebug() << "daemon started";
	return true;
}

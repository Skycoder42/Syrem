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

	if(parser.isSet(QStringLiteral("daemon"))) {
		qCritical() << "no qtmvvm app should be started when running as daemon";
		return false;
	}

	_roNode = new QRemoteObjectNode(this);
	_roNode->setName(QStringLiteral("widgets-main"));
	if(!_roNode->connectToNode(QUrl(QStringLiteral("local:remindme-daemon")))) {
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

void RemindMeApp::aboutToQuit()
{
	//if you need to perform any cleanups, do it here
}

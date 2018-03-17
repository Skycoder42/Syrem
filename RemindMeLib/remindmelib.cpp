#include "remindmelib.h"
#include <QJsonSerializer>
#include <QCoreApplication>

#include "schedule.h"
#include "dateparser.h"
#include "reminder.h"
#include "conflictresolver.h"

void RemindMe::setup(QtDataSync::Setup &setup)
{
	setup.setRemoteObjectHost(QStringLiteral("local:remindme-datasync")) //TODO make SSL with pw
			.setSyncPolicy(QtDataSync::Setup::PreferDeleted)
			.setRemoteConfiguration({QStringLiteral("wss://apps.skycoder42.de/datasync/")})
			.setConflictResolver(new ConflictResolver());
}

namespace {

void setupRemindMeLib()
{
	qRegisterMetaType<QList<QPair<int, ParserTypes::Expression::Span>>>("QList<QPair<int,ParserTypes::Expression::Span>>");
	qRegisterMetaType<ParserTypes::Datum*>();
	qRegisterMetaType<ParserTypes::Type*>();
	qRegisterMetaType<ParserTypes::TimePoint*>();
	qRegisterMetaType<Schedule*>();
	qRegisterMetaType<OneTimeSchedule*>();
	qRegisterMetaType<LoopSchedule*>();
	qRegisterMetaType<MultiSchedule*>();

	QJsonSerializer::registerAllConverters<Reminder>();
	QJsonSerializer::registerPointerConverters<Schedule>();
	QJsonSerializer::registerPairConverters<int, ParserTypes::Expression::Span>();
	QJsonSerializer::registerAllConverters<QPair<int, ParserTypes::Expression::Span>>();
}

}
Q_COREAPP_STARTUP_FUNCTION(setupRemindMeLib)

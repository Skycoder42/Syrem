#include "remindmelib_global.h"
#include <QJsonSerializer>
#include <QCoreApplication>

#include "schedule.h"
#include "dateparser.h"
#include "reminder.h"

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

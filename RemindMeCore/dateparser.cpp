#include "dateparser.h"
using namespace ReminderTypes;

Reminder::Reminder(QObject *parent) :
	QObject(parent)
{}

Datum::Datum(QObject *parent) :
	QObject(parent),
	scope(InvalidScope),
	value(0)
{}

QDate Datum::nextDate(QDate wDate) const
{
	if(scope == InvalidScope)
		return {};

	Q_ASSERT_X(value > 0, Q_FUNC_INFO, "invalid value, must be greater 1");
	switch (scope) {
	case WeekDayScope:
		Q_ASSERT_X(value <= 7, Q_FUNC_INFO, "invalid weekday value, must be at most 7");
		if(wDate.dayOfWeek() > value)
			wDate = wDate.addDays(7 - (wDate.dayOfWeek() - value));
		else
			wDate = wDate.addDays(value - wDate.dayOfWeek());
		break;
	case DayScope:
		Q_ASSERT_X(value <= 31, Q_FUNC_INFO, "invalid day value, must be at most 31");
		if(wDate.day() > value)
			wDate = wDate.addDays((wDate.daysInMonth() - wDate.day()) + 1);
		while(wDate.daysInMonth() < value)
			wDate = wDate.addMonths(1);
		wDate = wDate.addDays(value - wDate.day());
		break;
	case MonthScope:
		Q_ASSERT_X(value <= 12, Q_FUNC_INFO, "invalid month value, must be at most 12");
		if(wDate.month() > value)
			wDate = wDate.addMonths(12 - (wDate.month() - value));
		else
			wDate = wDate.addMonths(value - wDate.month());
		break;
	case MonthDayScope:
	{
		auto month = ((value >> 16) & 0x00FF);
		Q_ASSERT_X(month > 0, Q_FUNC_INFO, "invalid value, must be greater 1");
		Q_ASSERT_X(month <= 12, Q_FUNC_INFO, "invalid month value, must be at most 12");

		auto day = (value & 0x00FF);
		Q_ASSERT_X(day > 0, Q_FUNC_INFO, "invalid value, must be greater 1");
		Q_ASSERT_X(day <= 31, Q_FUNC_INFO, "invalid day value, must be at most 31");

		QDate nDate(wDate.year(), month, day);
		if(wDate > nDate)
			wDate.setDate(wDate.year() + 1, month, day);
		else
			wDate = nDate;
		break;
	}
	default:
		Q_UNREACHABLE();
		break;
	}

	return wDate;
}

Type::Type(QObject *parent) :
	QObject(parent),
	isDatum(false),
	datum(nullptr),
	span(Reminder::InvalidSpan)
{}

QDate Type::nextDate(QDate wDate) const
{
//	switch (dateType) {
//	case Day: //do nothing, simply take the day
//		break;
//	case Week:
//		if(wDate.dayOfWeek() != 1)
//			wDate = wDate.addDays(8 - wDate.dayOfWeek());
//		break;
//	case Month:
//		if(wDate.month() != 1)
//			wDate = wDate.addMonths(13 - wDate.month());
//		break;
//	case Year:
//		break;
//	case Datum:
//		wDate = nextDate(wDate);
//		break;
//	default:
//		Q_UNREACHABLE();
//		break;
//	}

	Q_UNIMPLEMENTED();
	return wDate;
}

TimePoint::TimePoint(QObject *parent) :
	QObject(parent),
	mode(InvalidMode),
	date(),
	datum(nullptr)
{}

QDate TimePoint::nextDate(QDate wDate) const
{
	Q_UNIMPLEMENTED();
	return wDate;
}

Conjunction::Conjunction(QObject *parent) :
	Reminder(parent),
	reminders()
{}

TimeSpan::TimeSpan(QObject *parent) :
	Reminder(parent),
	span(InvalidSpan),
	datum(nullptr),
	time()
{}

Loop::Loop(QObject *parent) :
	Reminder(parent),
	type(nullptr),
	datum(nullptr),
	time(),
	from(nullptr),
	until(nullptr)
{}

Point::Point(QObject *parent) :
	Reminder(parent),
	date(nullptr),
	time()
{}

DateParser::DateParser(QObject *parent) :
	QObject(parent)
{}

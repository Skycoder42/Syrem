#include "dateparser.h"
using namespace ReminderTypes;

Reminder::Reminder(QObject *parent) :
	QObject(parent)
{}

Datum::Datum(QObject *parent) :
	Reminder(parent),
	scope(DayScope),
	value(1),
	time()
{
	//TODO assert value ranges
}

QDateTime Datum::nextSchedule(const QDateTime &since)
{
	auto resDateTime = prepareDate(since);
	resDateTime.setDate(nextDate(resDateTime.date()));
	return resDateTime;
}

QDateTime Datum::prepareDate(QDateTime since)
{
	//in case the time is valid, but passed, or not valid at all, today cannot be the target anymore
	if(time.isValid()) {
		if(since.time() > time)
			since = since.addDays(1);
		since.setTime(time);
	} else {
		since = since.addDays(1);
		since.setTime(QTime());
	}

	return since;
}

QDate Datum::nextDate(QDate wDate)
{
	switch (scope) {
	case WeekDayScope:
		if(wDate.dayOfWeek() > value)
			wDate = wDate.addDays(7 - (wDate.dayOfWeek() - value));
		else
			wDate = wDate.addDays(value - wDate.dayOfWeek());
		break;
	case DayScope:
		if(wDate.day() > value)
			wDate = wDate.addDays((wDate.daysInMonth() - wDate.day()) + 1);
		while(wDate.daysInMonth() < value)
			wDate = wDate.addMonths(1);
		wDate = wDate.addDays(value - wDate.day());
		break;
	case MonthScope:
		if(wDate.month() > value)
			wDate = wDate.addMonths(12 - (wDate.month() - value));
		else
			wDate = wDate.addMonths(value - wDate.month());
		break;
	case MonthDayScope:
	{
		auto month = ((value >> 16) & 0x00FF);
		auto day = (value & 0x00FF);
		QDate nDate(wDate.year(), month, day);
		if(nDate < wDate)
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
	Datum(parent),
	dateType(Day)
{}

QDateTime Type::nextSchedule(const QDateTime &since)
{
	auto resDateTime = prepareDate(since);
	auto wDate = resDateTime.date();

	switch (dateType) {
	case Day: //do nothing, simply take the day
		break;
	case Week:
		if(wDate.dayOfWeek() != 1)
			wDate = wDate.addDays(8 - wDate.dayOfWeek());
		break;
	case Month:
		if(wDate.month() != 1)
			wDate = wDate.addMonths(13 - wDate.month());
		break;
	case Year:
		break;
	case Datum:
		wDate = nextDate(wDate);
		break;
	default:
		Q_UNREACHABLE();
		break;
	}

	resDateTime.setDate(wDate);
	return resDateTime;
}

TimePoint::TimePoint(QObject *parent) :
	Reminder(parent)
{}

DatePoint::DatePoint(QObject *parent) :
	TimePoint(parent),
	date()
{}

QDateTime DatePoint::nextSchedule(const QDateTime &since)
{
	if(since <= date)
		return date;
	else
		return {};
}

DatumPoint::DatumPoint(QObject *parent) :
	TimePoint(parent),
	datum(nullptr)
{}

Conjunction::Conjunction(QObject *parent) :
	Reminder(parent),
	reminders()
{}

Offset::Offset(QObject *parent) :
	Reminder(parent),
	datum(nullptr),
	timepoint(nullptr)
{}

Loop::Loop(QObject *parent) :
	Reminder(parent),
	datum(nullptr),
	timepoint(nullptr),
	from(nullptr),
	until(nullptr)
{}

Range::Range(QObject *parent) :
	Reminder(parent),
	from(nullptr),
	until(nullptr)
{}



DateParser::DateParser(QObject *parent) :
	QObject(parent)
{}

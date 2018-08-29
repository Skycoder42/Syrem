#include "dateparser.h"

#include <QLocale>
#include <QRegularExpression>
#include <chrono>
#include "syncedsettings.h"
using namespace ParserTypes;

Datum::Datum(QObject *parent) :
	QObject(parent),
	scope(InvalidScope),
	value(0)
{}

QDate Datum::nextDate(QDate wDate, bool scopeReset, bool notToday) const
{
	if(scope == InvalidScope)
		return {};

	Q_ASSERT_X(value > 0, Q_FUNC_INFO, "invalid value, must be greater 1");
	switch (scope) {
	case WeekDayScope:
		Q_ASSERT_X(value <= 7, Q_FUNC_INFO, "invalid weekday value, must be at most 7");
		if(scopeReset)
			wDate = wDate.addDays((wDate.dayOfWeek() - 1) * -1);
		else if(notToday)
			wDate = wDate.addDays(1);

		if(wDate.dayOfWeek() > value) //go to sunday + the target day
			wDate = wDate.addDays((7 - wDate.dayOfWeek()) + value);
		else if(wDate.dayOfWeek() < value) //simply add the missing days
			wDate = wDate.addDays(value - wDate.dayOfWeek());
		break;
	case DayScope:
		Q_ASSERT_X(value <= 31, Q_FUNC_INFO, "invalid day value, must be at most 31");
		if(scopeReset)
			wDate = wDate.addDays((wDate.day() - 1) * -1);
		else if(notToday)
			wDate = wDate.addDays(1);

		if(wDate.day() > value) //already past -> go to next month 1.
			wDate = wDate.addDays((wDate.daysInMonth() - wDate.day()) + 1);

		if(wDate.daysInMonth() < value)
			wDate = wDate.addDays(wDate.daysInMonth() - wDate.day());
		else if(wDate.day() < value)
			wDate = wDate.addDays(value - wDate.day());
		break;
	case MonthScope:
		Q_ASSERT_X(value <= 12, Q_FUNC_INFO, "invalid month value, must be at most 12");
		if(scopeReset)
			wDate = wDate.addMonths((wDate.month() - 1) * -1);
		else if(notToday)
			wDate = wDate.addMonths(1);

		if(wDate.month() > value)
			wDate = wDate.addMonths(12 - (wDate.month() - value));
		else if(wDate.month() < value)
			wDate = wDate.addMonths(value - wDate.month());
		break;
	case MonthDayScope:
	{
		if(!scopeReset && notToday)
			wDate = wDate.addDays(1);

		auto mDayPair = fromMonthDay(value);
		auto day = mDayPair.first;
		auto month = mDayPair.second;

		QDate nDate(wDate.year(), month, day);
		if(!scopeReset && wDate > nDate)
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

int Datum::toMonthDay(int day, int month)
{
	return ((month << 16) & 0xFFFF0000) |
			(day & 0x0000FFFF);
}

QPair<int, int> Datum::fromMonthDay(int monthDay)
{
	auto month = ((monthDay >> 16) & 0x0000FFFF);
	Q_ASSERT_X(month > 0, Q_FUNC_INFO, "invalid value, must be greater 1");
	Q_ASSERT_X(month <= 12, Q_FUNC_INFO, "invalid month value, must be at most 12");

	auto day = (monthDay & 0x0000FFFF);
	Q_ASSERT_X(day > 0, Q_FUNC_INFO, "invalid value, must be greater 1");
	Q_ASSERT_X(day <= 31, Q_FUNC_INFO, "invalid day value, must be at most 31");

	return {day, month};
}

QDateTime ParserTypes::nextSequenceDate(const Sequence &sequence, const QDateTime &since, bool *timeChange)
{
	using namespace std::chrono;
	if(sequence.isEmpty())
		return {};

	if(timeChange)
		*timeChange = false;
	auto res = since;
	for(auto span : sequence) {
		switch (span.second) {
		case Expression::InvalidSpan:
			return {};
		case Expression::MinuteSpan:
			if(timeChange)
				*timeChange = true;
			res = res.addSecs(duration_cast<seconds>(minutes(span.first)).count());
			break;
		case Expression::HourSpan:
			if(timeChange)
				*timeChange = true;
			res = res.addSecs(duration_cast<seconds>(hours(span.first)).count());
			break;
		case Expression::DaySpan:
			res = res.addDays(span.first);
			break;
		case Expression::WeekSpan:
			res = res.addDays(7 * span.first);
			break;
		case Expression::MonthSpan:
			res = res.addMonths(span.first);
			break;
		case Expression::YearSpan:
			res = res.addYears(span.first);
			break;
		default:
			Q_UNREACHABLE();
			break;
		}
	}

	return res;
}

Type::Type(QObject *parent) :
	QObject(parent),
	isDatum(false),
	datum(nullptr),
	sequence()
{}

QDateTime Type::nextDateTime(const QDateTime &since) const
{
	if(isDatum) {
		if(datum) {
			QDateTime tp;
			tp.setDate(datum->nextDate(since.date(), false, true));//just like for timepoint, the next possible except now
			tp.setTime(since.time());
			return tp;
		} else
			return {};
	} else
		return nextSequenceDate(sequence, since);
}

TimePoint::TimePoint(QObject *parent) :
	QObject(parent),
	mode(InvalidMode),
	date(),
	datum(nullptr)
{}

bool TimePoint::isLess(const TimePoint *other) const
{
	if(mode == InvalidMode || other->mode == InvalidMode)
		return true;
	if(mode == DatumMode || other->mode == DatumMode)
		return false;

	if(mode == DateMode && other->mode == DateMode)
		return date < other->date;
	else
		return date.year() < other->date.year();
}

QDate TimePoint::nextDate(QDate wDate, bool notToday) const
{
	switch (mode) {
	case TimePoint::InvalidMode:
		return {};
	case TimePoint::DateMode:
		if(wDate <= date)
			return date;
		else
			return {};
	case TimePoint::DatumMode:
		if(datum)
			return datum->nextDate(wDate, false, notToday); //no scope reset, but not today/this month -> the next possible occurance
		else
			return {};
	case TimePoint::YearMode:
		if(wDate.year() < date.year())
			return wDate.addYears(date.year() - wDate.year());
		else
			return {};
	default:
		Q_UNREACHABLE();
		return {};
	}
}


#include "dateparser.h"

#include <QLocale>
#include <QRegularExpression>
using namespace ParserTypes;

Expression::Expression(QObject *parent) :
	QObject(parent)
{}

QDateTime Expression::nextSpanDate(Expression::Span span, int count, const QDateTime &since)
{
	switch (span) {
	case InvalidSpan:
		return {};
	case MinuteSpan:
		return since.addSecs(60 * count);
	case HourSpan:
		return since.addSecs(60 * 60 * count); //TODO TEST check boundaries
	case DaySpan:
		return since.addDays(count);
	case WeekSpan:
		return since.addDays(7 * count);
	case MonthSpan:
		return since.addMonths(count);
	case YearSpan:
		return since.addYears(count);
	default:
		Q_UNREACHABLE();
		break;
	}
}

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

Type::Type(QObject *parent) :
	QObject(parent),
	isDatum(false),
	datum(nullptr),
	count(0),
	span(Expression::InvalidSpan)
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
		return Expression::nextSpanDate(span, count, since);
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

QDate TimePoint::nextDate(QDate wDate) const
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
			return datum->nextDate(wDate, false, true); //no scope reset, but not today/this month -> the next possible occurance
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

Conjunction::Conjunction(QObject *parent) :
	Expression(parent),
	expressions()
{}

Schedule *Conjunction::createSchedule(const QDateTime &since, QObject *parent)
{
	auto schedule = new MultiSchedule(parent);
	foreach(auto expr, expressions)
		schedule->addSubSchedule(expr->createSchedule(since, schedule));
	return schedule;
}

TimeSpan::TimeSpan(QObject *parent) :
	Expression(parent),
	span(InvalidSpan),
	count(0),
	datum(nullptr),
	time()
{}

Schedule *TimeSpan::createSchedule(const QDateTime &since, QObject *parent)
{
	QDateTime tp = nextSpanDate(span, count, since);

	//apply datum/time, if set (assume valid, as after parsing)
	if(datum)
		tp.setDate(datum->nextDate(tp.date(), true)); //with scope reset, because the target "scope" is already given
	if(time.isValid())
		tp.setTime(time);

	return new OneTimeSchedule(tp, parent);
}

Loop::Loop(QObject *parent) :
	Expression(parent),
	type(nullptr),
	datum(nullptr),
	time(),
	from(nullptr),
	fromTime(),
	until(nullptr),
	untilTime()
{}

Schedule *Loop::createSchedule(const QDateTime &since, QObject *parent)
{
	auto sched = new LoopSchedule(parent);

	sched->type = type;
	sched->datum = datum;
	sched->time = time;

	if(from)
		sched->from.setDate(from->nextDate(since.date()));

	if(until && from)
		sched->until.setDate(sched->from.date());//calculate from "from" on, because of relative datum values
	else if(until)
		sched->until.setDate(until->nextDate(since.date()));

	if(fromTime.isValid())
		sched->from.setTime(fromTime);
	if(untilTime.isValid())
		sched->until.setTime(untilTime);

	return sched;
}

Point::Point(QObject *parent) :
	Expression(parent),
	date(nullptr),
	time()
{}

Schedule *Point::createSchedule(const QDateTime &since, QObject *parent)
{
	QDateTime tp;

	tp.setDate(date->nextDate(since.date()));
	if(!tp.date().isValid())
		return nullptr;
	if(time.isValid())
		tp.setTime(time);
	if(tp <= since)
		return nullptr;

	return new OneTimeSchedule(tp, parent);
}

DateParser::DateParser(QObject *parent) :
	QObject(parent)
{}



const QString DateParser::timeRegex = QStringLiteral(R"__((?:at )?(\d{1,2}:\d{2}|\d{1,2} oclock))__");

Expression *DateParser::parse(const QString &data)
{
	auto dummyParent = new QObject();
	try {
		auto expr = parseExpression(data, dummyParent);
		expr->setParent(nullptr);
		dummyParent->deleteLater();
		return expr;
	} catch(QString &s) {
		//TODO use s
		qCritical(qUtf8Printable(s));
		dummyParent->deleteLater();
		return nullptr;
	}
}

Expression *DateParser::parseExpression(const QString &data, QObject *parent)
{
	Expression *expr = nullptr;

	expr = tryParseConjunction(data, parent);
	if(expr)
		return expr;

	expr = tryParseTimeSpan(data, parent);
	if(expr)
		return expr;

	expr = tryParseLoop(data, parent);
	if(expr)
		return expr;

	expr = tryParsePoint(data, parent);
	if(expr)
		return expr;

	throw tr("Not an expression");
}

Conjunction *DateParser::tryParseConjunction(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__((\s+and\s+|\s*;\s*))__"),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption |
										  QRegularExpression::DontCaptureOption);
	auto list = data.split(regex);
	if(list.size() == 1)
		return nullptr;

	auto con = new Conjunction(parent);
	foreach (auto expr, list)
		con->expressions.append(parseExpression(expr, con));
	return con;
}

TimeSpan *DateParser::tryParseTimeSpan(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__(^%1 (\d+) (\w+)(?:(?:%2) (.+?))??(?: %3)?$)__")
										  .arg(tr("in"))
										  .arg(tr(" on| at| in"))
										  .arg(timeRegex),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		auto ts = new TimeSpan(parent);
		ts->count = match.captured(1).toInt();
		if(ts->count < 1)
			throw tr("Cannot use in 0 days");
		ts->span = parseSpan(match.captured(2));
		auto dateStr = match.captured(3);
		if(!dateStr.isEmpty())
			ts->datum = parseDatum(dateStr, ts);
		auto timeStr = match.captured(4);
		if(!timeStr.isEmpty())
			ts->time = parseTime(timeStr);

		//validate the given datum is "logical" for the given span
		validateSpanDatum(ts->span, ts->datum, ts->time);

		return ts;
	} else
		return nullptr;
}

Loop *DateParser::tryParseLoop(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__(^%1 (.+?)(?:(?:%2) (.+?))??(?: %3)?(?: %4 ((?:(?!%5).)*))?(?: %5 (.*))?$)__")
										  .arg(tr("every"))
										  .arg(tr(" on| at| in"))
										  .arg(timeRegex)
										  .arg(tr("from"))
										  .arg(tr("until")),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		auto loop = new Loop(parent);
		loop->type = parseType(match.captured(1), loop);
		auto dateStr = match.captured(2);
		if(!dateStr.isEmpty())
			loop->datum = parseDatum(dateStr, loop);
		auto timeStr = match.captured(3);
		if(!timeStr.isEmpty())
			loop->time = parseTime(timeStr);
		auto fromStr = match.captured(4);
		if(!fromStr.isEmpty()) {
			auto pair = parseExtendedTimePoint(fromStr, loop);
			loop->from = pair.first;
			loop->fromTime = pair.second;
		}
		auto untilStr = match.captured(5);
		if(!untilStr.isEmpty()) {
			auto pair = parseExtendedTimePoint(untilStr, loop);
			loop->until = pair.first;
			loop->untilTime = pair.second;
		}

		if(loop->type->isDatum)
			validateDatumDatum(loop->type->datum, loop->datum);
		else
			validateSpanDatum(loop->type->span, loop->datum, loop->time);

		if(loop->from && loop->until) {
			if(loop->until->isLess(loop->from))
				throw tr("from must be an earlier timepoint than until");
		}

		return loop;
	} else
		return nullptr;
}

Point *DateParser::tryParsePoint(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__(^(?:%1)?(.+?)(?: %2)?$)__")
										  .arg(tr("on |next "))
										  .arg(timeRegex),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		auto pnt = new Point(parent);
		pnt->date = parseTimePoint(match.captured(1), pnt);
		auto timeStr = match.captured(2);
		if(!timeStr.isEmpty())
			pnt->time = parseTime(timeStr);
		return pnt;
	} else
		return nullptr;
}

Datum *DateParser::parseDatum(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__(^(?:(%1)|(\d+)\.|(%2)|(.+?))$)__")
										  .arg(readWeekDays().keys().join(QStringLiteral("|")))
										  .arg(readMonths().keys().join(QStringLiteral("|"))),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		auto datum = new Datum(parent);

		//weekdays
		auto mRes = match.captured(1);
		if(!mRes.isEmpty()) {
			datum->scope = Datum::WeekDayScope;
			datum->value = readWeekDays().value(mRes.toLower().trimmed(), -1);
			if(datum->value == -1)
				throw tr("Unable to match found weekday to day number");
			return datum;
		}

		//days
		mRes = match.captured(2);
		if(!mRes.isEmpty()) {
			auto day = mRes.toInt();
			if(day < 1 || day > 31)
				throw tr("Parsed day value out of valid day range");
			datum->scope = Datum::DayScope;
			datum->value = day;
			return datum;
		}

		//month names
		mRes = match.captured(3);
		if(!mRes.isEmpty()) {
			datum->scope = Datum::MonthScope;
			datum->value = readMonths().value(mRes.toLower().trimmed(), -1);
			if(datum->value == -1)
				throw tr("Unable to match found month to month number");
			return datum;
		}

		//monthday
		mRes = match.captured(4);
		if(!mRes.isEmpty()) {
			auto date = parseMonthDay(mRes, true);
			if(date.isValid()) {
				datum->scope = Datum::MonthDayScope;
				datum->value = Datum::toMonthDay(date.day(), date.month());
				return datum;
			}
		}
	}

	throw tr("Invalid datum specified");
}

Type *DateParser::parseType(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__(^(?:(\d+) (\w+)|(.+?))$)__"),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		auto type = new Type(parent);
		auto span = match.captured(2);
		if(!span.isEmpty()) {
			type->isDatum = true;
			type->count = match.captured(1).toInt();
			if(type->count < 1)
				throw tr("Cannot use in 0 days");
			type->span = parseSpan(span);
		} else {
			type->isDatum = true;
			type->datum = parseDatum(match.captured(3), type);
		}
		return type;
	} else
		throw tr("Invalid type specified");
}

TimePoint *DateParser::parseTimePoint(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__(^(?:(%1)|(%2)|(\d{4})|(.*?))$)__")
										  .arg(tr("today"))
										  .arg(tr("tomorrow")),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		auto tp = new TimePoint(parent);

		//today
		auto mRes = match.captured(1);
		if(!mRes.isEmpty()) {
			tp->mode = TimePoint::DateMode;
			tp->date = QDate::currentDate();
		}

		//tomorrow
		mRes = match.captured(2);
		if(!mRes.isEmpty()) {
			tp->mode = TimePoint::DateMode;
			tp->date = QDate::currentDate().addDays(1);
		}

		//year
		mRes = match.captured(3);
		if(!mRes.isEmpty()) {
			tp->mode = TimePoint::YearMode;
			tp->date.setDate(mRes.toInt(), 1, 1);
		}

		//date/datum
		mRes = match.captured(4);
		if(!mRes.isEmpty()) {
			auto date = parseDate(mRes, true);
			if(date.isValid()) {
				tp->mode = TimePoint::DateMode;
				tp->date = date;
			} else {
				tp->mode = TimePoint::DatumMode;
				tp->datum = parseDatum(mRes, tp);
			}
		}

		return tp;
	} else
		throw tr("Invalid type specified");
}

QPair<TimePoint *, QTime> DateParser::parseExtendedTimePoint(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__(^(.+?)(?: %1)?$)__")
										  .arg(timeRegex),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		QPair<TimePoint*, QTime> pair;
		pair.first = parseTimePoint(match.captured(1), parent);
		auto time = match.captured(2);
		if(!time.isEmpty())
			pair.second = parseTime(time);
		return pair;
	} else
		throw tr("Invalid timepoint and/or time");
}

QDate DateParser::parseMonthDay(const QString &data, bool noThrow)
{
	QLocale locale;
	auto dates = tr("d.M.|dd.M.|d.MM.|dd.MM.|d. M.|dd. M.|d. MM.|dd. MM.|d. MMM|d. MMMM|dd. MMM|dd. MMMM|d-M|d-MM|dd-M|dd-MM").split(QStringLiteral("|"));
	foreach(auto pattern, dates) {
		auto date = locale.toDate(data, pattern);
		if(date.isValid())
			return date;
	}

	if(noThrow)
		return {};
	else
		throw tr("Invalid month-day specified");
}

QDate DateParser::parseDate(const QString &data, bool noThrow)
{
	QLocale locale;
	auto dates = tr("d. M. yyyy|dd. M. yyyy|d. MM. yyyy|dd. MM. yyyy|d. MMM yyyy|d. MMMM yyyy|dd. MMM yyyy|dd. MMMM yyyy|d-M-yyyy|d-MM-yyyy|dd-M-yyyy|dd-MM-yyyy").split(QStringLiteral("|"));
	foreach(auto pattern, dates) {
		auto date = locale.toDate(data, pattern);
		if(date.isValid())
			return date;
	}

	if(noThrow)
		return {};
	else
		throw tr("Invalid date specified");
}

QTime DateParser::parseTime(const QString &data)
{
	QLocale locale;
	auto times = tr("hh:mm|h:mm|h' oclock'").split(QStringLiteral("|"));
	foreach(auto pattern, times) {
		auto time = locale.toTime(data, pattern);
		if(time.isValid())
			return time;
	}

	throw tr("Invalid time specified");
}

#define SPAN_REGEX(x) QRegularExpression(QStringLiteral("(?:%1)").arg(x), \
	QRegularExpression::OptimizeOnFirstUsageOption | \
	QRegularExpression::CaseInsensitiveOption | \
	QRegularExpression::DontCaptureOption)

Expression::Span DateParser::parseSpan(const QString &data)
{
	static const QHash<QRegularExpression, Expression::Span> spanMap = {
		{SPAN_REGEX(tr("minute|minutes")), Expression::MinuteSpan},
		{SPAN_REGEX(tr("hour|hours")), Expression::HourSpan},
		{SPAN_REGEX(tr("day|days")), Expression::DaySpan},
		{SPAN_REGEX(tr("week|weeks")), Expression::WeekSpan},
		{SPAN_REGEX(tr("month|months")), Expression::MonthSpan},
		{SPAN_REGEX(tr("year|years")), Expression::YearSpan},
	};

	auto sData = data.simplified();
	for(auto it = spanMap.begin(); it != spanMap.end(); it++) {
		auto match = it.key().match(sData);
		if(match.hasMatch())
			return it.value();
	}

	throw tr("Invalid time span");
}

void DateParser::validateDatumDatum(Datum *datum, const Datum *extraDatum)
{
	if(!datum)
		throw tr("Invalid datum");

	switch (datum->scope) {
	case Datum::InvalidScope:
		throw tr("Invalid scope");
	case Datum::WeekDayScope:
	case Datum::DayScope:
	case Datum::MonthDayScope:
		if(extraDatum)
			throw tr("You cannot specify a datum for a scope of less then a month");
		break;
	case Datum::MonthScope:
		if(extraDatum && extraDatum->scope > Datum::DayScope)
			throw tr("You cannot specify a datum for month(day)s on a span of a month");
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

void DateParser::validateSpanDatum(Expression::Span span, const Datum *datum, const QTime &time)
{
	switch (span) {
	case Expression::InvalidSpan:
		throw tr("Invalid span");
	case Expression::MinuteSpan:
	case Expression::HourSpan:
		if(time.isValid())
			throw tr("You cannot specify a time for a span less then a day");
		Q_FALLTHROUGH();
	case Expression::DaySpan:
		if(datum)
			throw tr("You cannot specify a datum for a span less then a week");
		break;
	case Expression::WeekSpan:
		if(datum && datum->scope > Datum::WeekDayScope)
			throw tr("You cannot specify a datum for more than weekdays on a span of a week");
		break;
	case Expression::MonthSpan:
		if(datum && datum->scope > Datum::DayScope)
			throw tr("You cannot specify a datum for month(day)s on a span of a month");
		break;
	case Expression::YearSpan:
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

QMap<QString, int> DateParser::readWeekDays()
{
	QLocale locale;
	QMap<QString, int> dayList;
	for(int i = Qt::Monday; i <= Qt::Sunday; i++) {
		dayList.insert(locale.standaloneDayName(i, QLocale::LongFormat).toLower().trimmed(), i);
		dayList.insert(locale.standaloneDayName(i, QLocale::ShortFormat).toLower().trimmed(), i);
	}
	return dayList;
}

QMap<QString, int> DateParser::readMonths()
{
	QLocale locale;
	QMap<QString, int> monthList;
	for(auto i = 1; i <= 12; i++) {
		monthList.insert(locale.standaloneMonthName(i, QLocale::LongFormat).toLower().trimmed(), i);
		monthList.insert(locale.standaloneMonthName(i, QLocale::ShortFormat).toLower().trimmed(), i);
	}
	return monthList;
}

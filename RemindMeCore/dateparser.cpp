#include "dateparser.h"

#include <QLocale>
#include <QRegularExpression>
using namespace ReminderTypes;

Expression::Expression(QObject *parent) :
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
	count(0),
	span(Expression::InvalidSpan)
{}

QDate Type::nextDate(QDate wDate) const
{
	if(isDatum) {
		if(datum)
			return datum->nextDate(wDate);
		else
			return {};
	}

	//TODO update with count
	switch (span) {
	case Expression::InvalidSpan:
		return {};
	case Expression::MinuteSpan:
	case Expression::HourSpan:
	case Expression::DaySpan:
		break;
	case Expression::WeekSpan:
		if(wDate.dayOfWeek() != 1)
			wDate = wDate.addDays(8 - wDate.dayOfWeek());
		break;
	case Expression::MonthSpan:
		if(wDate.month() != 1)
			wDate = wDate.addMonths(13 - wDate.month());
		break;
	case Expression::YearSpan:
		break;
	default:
		Q_UNREACHABLE();
		break;
	}

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
	switch (mode) {
	case TimePoint::InvalidMode:
		return {};
	case TimePoint::DateMode:
		if(wDate <= date.date())
			return date.date();
		else
			return {};
	case TimePoint::DatumMode:
		if(datum)
			return datum->nextDate(wDate);
		else
			return {};
	case TimePoint::YearMode:
		return wDate;
	default:
		Q_UNREACHABLE();
		return {};
	}
}

Conjunction::Conjunction(QObject *parent) :
	Expression(parent),
	expressions()
{}

TimeSpan::TimeSpan(QObject *parent) :
	Expression(parent),
	span(InvalidSpan),
	count(0),
	datum(nullptr),
	time()
{}

Loop::Loop(QObject *parent) :
	Expression(parent),
	type(nullptr),
	datum(nullptr),
	time(),
	from(nullptr),
	until(nullptr)
{}

Point::Point(QObject *parent) :
	Expression(parent),
	date(nullptr),
	time()
{}

DateParser::DateParser(QObject *parent) :
	QObject(parent)
{}



const QString DateParser::timeRegex = QStringLiteral(R"__((?:at )?(\d{1,2}:\d{2}|\d{1,2} oclock))__");

Expression *DateParser::parse(const QString &data)
{
	try {
		return parseExpression(data, nullptr);//TODO
	} catch(...) {
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
	static const QRegularExpression regex(QStringLiteral(R"__(^%1 (\d+) (\w+)(?:(?:%2) (.+?))?(?: %3)?$)__")
										  .arg(tr("in"))
										  .arg(tr(" on| at| in"))
										  .arg(timeRegex),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		auto ts = new TimeSpan(parent);
		ts->count = match.captured(1).toInt();
		ts->span = parseSpan(match.captured(2));
		auto dateStr = match.captured(3);
		if(!dateStr.isEmpty())
			ts->datum = parseDatum(dateStr, ts);
		auto timeStr = match.captured(4);
		if(!timeStr.isEmpty())
			ts->time = parseTime(timeStr);
		return ts;
	} else
		return nullptr;
}

Loop *DateParser::tryParseLoop(const QString &data, QObject *parent)
{
	static const QRegularExpression regex(QStringLiteral(R"__(^%1 (.+?)(?:(?:%2) (.+?))?(?: %3)?(?: %4 ((?:(?!%5).)*))?(?: %5 (.*))?$)__")
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
		if(!fromStr.isEmpty())
			loop->from = parseTimePoint(fromStr, loop);
		auto untilStr = match.captured(5);
		if(!untilStr.isEmpty())
			loop->until = parseTimePoint(untilStr, loop);
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
	static const QRegularExpression regex(QStringLiteral(R"__(^(?:(%1)|(\d+)\.|(\d+)|(%2)|(.+?))$)__")
										  .arg(readWeekDays().join(QStringLiteral("|")))
										  .arg(readMonths().join(QStringLiteral("|"))),
										  QRegularExpression::OptimizeOnFirstUsageOption |
										  QRegularExpression::CaseInsensitiveOption);

	auto match = regex.match(data.simplified());
	if(match.hasMatch()) {
		auto datum = new Datum(parent);

		//weekdays
		auto mRes = match.captured(1);
		if(!mRes.isEmpty()) {
			auto days = readWeekDays();
			auto idx = days.indexOf(mRes.toLower().trimmed());
			if(idx == -1)
				throw tr("Unable to match found weekday to day number");
			datum->scope = Datum::WeekDayScope;
			datum->value = idx + 1;
		}

		//days
		mRes = match.captured(2);
		if(!mRes.isEmpty()) {
			auto day = mRes.toInt();
			if(day < 1 || day > 31)
				throw tr("Parsed day value out of valid day range");
			datum->scope = Datum::DayScope;
			datum->value = day;
		}

		//months
		mRes = match.captured(3);
		if(!mRes.isEmpty()) {
			auto month = mRes.toInt();
			if(month < 1 || month > 12)
				throw tr("Parsed month value out of valid month range");
			datum->scope = Datum::MonthScope;
			datum->value = month;
		}

		//month names
		mRes = match.captured(4);
		if(!mRes.isEmpty()) {
			auto months = readMonths();
			auto idx = months.indexOf(mRes.toLower().trimmed());
			if(idx == -1)
				throw tr("Unable to match found month to month number");
			datum->scope = Datum::MonthScope;
			datum->value = idx + 1;
		}

		//monthday
		mRes = match.captured(5);
		if(!mRes.isEmpty()) {
			auto date = parseMonthDay(mRes);
			datum->scope = Datum::MonthDayScope;
			datum->value = ((date.month() << 16) & 0xFF00) |
						   (date.day() & 0x00FF);
		}

		return datum;
	} else
		throw tr("Invalid datum specified");
}

Type *DateParser::parseType(const QString &data, QObject *parent)
{

}

TimePoint *DateParser::parseTimePoint(const QString &data, QObject *parent)
{

}

QDate DateParser::parseMonthDay(const QString &data)
{
	auto dates = tr("d. M.|dd. M.|d. MM.|dd. MM.|d. MMM|d. MMMM|dd. MMM|dd. MMMM|d-M|d-MM|dd-M|dd-MM").split(QStringLiteral("|"));
	foreach(auto pattern, dates) {
		auto date = QDate::fromString(data, pattern);
		if(date.isValid())
			return date;
	}

	throw tr("Invalid date specified");
}

QDate DateParser::parseDate(const QString &data)
{

}

QTime DateParser::parseTime(const QString &data)
{
	auto times = tr("hh:mm|h:mm|h' oclock'").split(QStringLiteral("|"));
	foreach(auto pattern, times) {
		auto time = QTime::fromString(data, pattern);
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

QStringList DateParser::readWeekDays()
{
	QStringList dayList;
	for(int i = Qt::Monday; i <= Qt::Sunday; i++)
		dayList.append(QDate::longDayName(i, QDate::StandaloneFormat).toLower().trimmed());
	return dayList;
}

QStringList DateParser::readMonths()
{
	QStringList dayList;
	for(auto i = 1; i <= 12; i++)
		dayList.append(QDate::longMonthName(i, QDate::StandaloneFormat).toLower().trimmed());
	return dayList;
}

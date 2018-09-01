#ifndef DATEPARSER_H
#define DATEPARSER_H

#include <QObject>
#include <QDateTime>
#include <QSharedPointer>
#include <QException>
#include <QtMvvmCore/Injection>

#include "libsyrem_global.h"
#include "schedule.h"
#include "syncedsettings.h"

namespace ParserTypes {

class LIB_SYREM_EXPORT Expression : public QObject
{
	Q_OBJECT

public:
	enum Span {
		InvalidSpan,
		MinuteSpan,
		HourSpan,
		DaySpan,
		WeekSpan,
		MonthSpan,
		YearSpan
	};
	Q_ENUM(Span)
};

// ------------- Basic Types -------------

class LIB_SYREM_EXPORT Datum : public QObject
{
	Q_OBJECT

	Q_PROPERTY(Scope scope MEMBER scope)
	Q_PROPERTY(int value MEMBER value)

public:
	enum Scope {
		InvalidScope,
		WeekDayScope,
		DayScope,
		MonthScope,
		MonthDayScope
	};
	Q_ENUM(Scope)

	Q_INVOKABLE Datum(QObject *parent = nullptr);

	QDate nextDate(QDate wDate, bool scopeReset = false, bool notToday = false) const;

	Scope scope;
	int value;

	static int toMonthDay(int day, int month);
	static QPair<int, int> fromMonthDay(int monthDay);
};

typedef QList<QPair<int, Expression::Span>> Sequence;
LIB_SYREM_EXPORT QDateTime nextSequenceDate(const Sequence &sequence, const QDateTime &since, bool *timeChange = nullptr);

class LIB_SYREM_EXPORT Type : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool isDatum MEMBER isDatum)
	Q_PROPERTY(ParserTypes::Datum* datum MEMBER datum)
	Q_PROPERTY(QList<QPair<int,ParserTypes::Expression::Span>> sequence MEMBER sequence) //dont use typedef to not confuse json serializer
	;

public:
	Q_INVOKABLE Type(QObject *parent = nullptr);

	QDateTime nextDateTime(const QDateTime &since) const;

	bool isDatum;
	Datum *datum;
	Sequence sequence;
};

class LIB_SYREM_EXPORT TimePoint : public QObject
{
	Q_OBJECT

	Q_PROPERTY(Mode mode MEMBER mode)
	Q_PROPERTY(QDate date MEMBER date)
	Q_PROPERTY(ParserTypes::Datum* datum MEMBER datum)

public:
	enum Mode {
		InvalidMode,
		DateMode,
		DatumMode,
		YearMode
	};
	Q_ENUM(Mode)

	Q_INVOKABLE TimePoint(QObject *parent = nullptr);

	bool isLess(const TimePoint *other) const;
	QDate nextDate(QDate wDate, bool notToday = true) const;

	Mode mode;
	QDate date;
	Datum *datum;
};

} //ParserTypes

Q_DECLARE_METATYPE(ParserTypes::Datum*)
Q_DECLARE_METATYPE(ParserTypes::Type*)
Q_DECLARE_METATYPE(ParserTypes::TimePoint*)

#endif // DATEPARSER_H

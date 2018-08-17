#ifndef EVENTEXPRESSIONPARSER_H
#define EVENTEXPRESSIONPARSER_H

#include <QObject>
#include <QtMvvmCore/Injection>

#include "remindmelib_global.h"
#include <syncedsettings.h>

namespace Expressions {

enum TypeFlag {
	InvalidType = 0x00,

	FlagAbsolute = 0x01,
	FlagRelative = 0x02,
	FlagTimepoint = 0x04,
	FlagTimespan = 0x08,
	FlagLooped = 0x10,

	AbsoluteTimepoint = FlagAbsolute | FlagTimepoint,
	RelativeTimepoint = FlagRelative | FlagTimepoint,
	Timespan = FlagRelative | FlagTimespan,
	LoopedTimePoint = RelativeTimepoint | FlagLooped,
	LoopedTimeSpan = Timespan | FlagLooped

	// NOTE do not allaw spans after timepoints. i.e. "in 3 months on 24." is ok, but "in june in 5 days" is not
	// NOTE ... and when in loops, use the timepoints as "from+until" restriction
};
Q_DECLARE_FLAGS(Type, TypeFlag)

enum ScopeFlag {
	InvalidScope = 0x00,

	Year = 0x01,
	Month = 0x02,
	Week = 0x04,
	Day = 0x08,
	WeekDay = Day,
	MonthDay = Week | Day,
	Hour = 0x10,
	Minute = 0x20,
};
Q_DECLARE_FLAGS(Scope, ScopeFlag)

enum WordKey {
	TimePrefix,
	TimeSuffix,
	TimePattern,

	DatePrefix,
	DateSuffix,
	DatePattern,

	InvTimeExprPattern,
	InvTimeHourPattern,
	InvTimeMinutePattern,
	InvTimeKeyword,

	MonthDayPrefix,
	MonthDaySuffix,
	MonthDayLoopPrefix,
	MonthDayLoopSuffix,
	MonthDayIndicator,

	MonthPrefix,
	MonthSuffix,
	MonthLoopPrefix,
	MonthLoopSuffix,

	YearPrefix,
	YearSuffix
};

} // break namespace to declare flag operators

Q_DECLARE_OPERATORS_FOR_FLAGS(Expressions::Type)
Q_DECLARE_OPERATORS_FOR_FLAGS(Expressions::Scope)

namespace Expressions { //.. and continue it

class REMINDMELIBSHARED_EXPORT SubTerm
{
	Q_DISABLE_COPY(SubTerm)
public:
	inline SubTerm(Type t = InvalidType, Scope s = InvalidScope, bool c = false) :
		type{t},
		scope{s},
		certain{c}
	{}
	virtual ~SubTerm();

	Type type;
	Scope scope;
	bool certain;

	virtual void apply(QDateTime &datetime, bool applyRelative) const = 0;
};

// sub-expession terms that use Qt date/time formats

class REMINDMELIBSHARED_EXPORT TimeTerm : public SubTerm //TODO add support for "20 past/before 3" via extra subterm
{
public:
	TimeTerm(QTime time, bool certain);
	static std::pair<QSharedPointer<TimeTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool keepOffset) const override;

private:
	QTime _time;

	static QString toRegex(QString pattern);
};

class REMINDMELIBSHARED_EXPORT DateTerm : public SubTerm
{
public:
	DateTerm(QDate date, bool hasYear, bool certain);
	static std::pair<QSharedPointer<DateTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyRelative) const override;

private:
	QDate _date;

	static QString toRegex(QString pattern, bool &hasYear);
};

// time expression for worded times like "10 past 11"
class REMINDMELIBSHARED_EXPORT InvertedTimeTerm : public SubTerm
{
public:
	InvertedTimeTerm(QTime time);
	static std::pair<QSharedPointer<InvertedTimeTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyRelative) const override;

private:
	QTime _time;

	static QString hourToRegex(QString pattern);
	static QString minToRegex(QString pattern);
};

class REMINDMELIBSHARED_EXPORT MonthDayTerm : public SubTerm
{
public:
	MonthDayTerm(int day, bool looped, bool certain);
	static std::pair<QSharedPointer<MonthDayTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyRelative) const override;

private:
	int _day;
};

class REMINDMELIBSHARED_EXPORT MonthTerm : public SubTerm
{
public:
	MonthTerm(int month, bool looped, bool certain);
	static std::pair<QSharedPointer<MonthTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyRelative) const override;

private:
	int _month;
};

class REMINDMELIBSHARED_EXPORT YearTerm : public SubTerm
{
public:
	YearTerm(int year, bool certain);
	static std::pair<QSharedPointer<YearTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyRelative) const override;

private:
	int _year;
};

// general helper method
QString trWord(WordKey key, bool escape = true);
QStringList trList(WordKey key, bool escape = true);

QString dateTimeFormatToRegex(QString pattern, const std::function<void(QString&)> &replacer);

}

class REMINDMELIBSHARED_EXPORT EventExpressionParser : public QObject
{
	Q_OBJECT

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	Q_INVOKABLE explicit EventExpressionParser(QObject *parent = nullptr);

	//QSharedPointer<Schedule> parseSchedule(const QString &expression);
	//QDateTime parseSnoozeTime(const QString &expression);

private:
	SyncedSettings *_settings = nullptr;
};

#endif // EVENTEXPRESSIONPARSER_H

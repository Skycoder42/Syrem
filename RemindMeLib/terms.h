#ifndef TERMS_H
#define TERMS_H

#include "eventexpressionparser.h"

namespace Expressions {

enum WordKey {
	TimePrefix,
	TimeSuffix,
	TimePattern,

	DatePrefix,
	DateSuffix,
	DateLoopPrefix,
	DateLoopSuffix,
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

	WeekDayPrefix,
	WeekDaySuffix,
	WeekDayLoopPrefix,
	WeekDayLoopSuffix,

	MonthPrefix,
	MonthSuffix,
	MonthLoopPrefix,
	MonthLoopSuffix,

	YearPrefix,
	YearSuffix,

	SpanPrefix,
	SpanSuffix,
	SpanLoopPrefix,
	SpanConjuction,
	SpanKeyMinute,
	SpanKeyHour,
	SpanKeyDay,
	SpanKeyWeek,
	SpanKeyMonth,
	SpanKeyYear,

	KeywordDayspan,

	LimiterFromPrefix,
	LimiterUntilPrefix,

	ExpressionSeperator
};

class REMINDMELIBSHARED_EXPORT TimeTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(QTime time MEMBER _time)

public:
	TimeTerm(QTime time);
	Q_INVOKABLE explicit TimeTerm(QObject *parent);
	static std::pair<QSharedPointer<TimeTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool keepOffset) const override;
	void fixup(QDateTime &datetime) const override;

private:
	QTime _time;

	static QString toRegex(QString pattern);
};

class REMINDMELIBSHARED_EXPORT DateTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(QDate date MEMBER _date)

public:
	DateTerm(QDate date, bool hasYear, bool isLooped);
	Q_INVOKABLE explicit DateTerm(QObject *parent);
	static std::pair<QSharedPointer<DateTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;
	void fixup(QDateTime &datetime) const override;

private:
	QDate _date;

	static QString toRegex(QString pattern, bool &hasYear);
};

class REMINDMELIBSHARED_EXPORT InvertedTimeTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(QTime time MEMBER _time)

public:
	InvertedTimeTerm(QTime time);
	Q_INVOKABLE explicit InvertedTimeTerm(QObject *parent);
	static std::pair<QSharedPointer<InvertedTimeTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;
	void fixup(QDateTime &datetime) const override;

private:
	QTime _time;

	static QString hourToRegex(QString pattern);
	static QString minToRegex(QString pattern);
};

class REMINDMELIBSHARED_EXPORT MonthDayTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(int day MEMBER _day)

public:
	MonthDayTerm(int day, bool looped);
	Q_INVOKABLE explicit MonthDayTerm(QObject *parent);
	static std::pair<QSharedPointer<MonthDayTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;
	void fixup(QDateTime &datetime) const override;

private:
	int _day;
};

class REMINDMELIBSHARED_EXPORT WeekDayTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(int weekDay MEMBER _weekDay)

public:
	WeekDayTerm(int weekDay, bool looped);
	Q_INVOKABLE explicit WeekDayTerm(QObject *parent);
	static std::pair<QSharedPointer<WeekDayTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;
	void fixup(QDateTime &datetime) const override;
	void fixupCleanup(QDateTime &datetime) const override;

private:
	int _weekDay;
};

class REMINDMELIBSHARED_EXPORT MonthTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(int month MEMBER _month)

public:
	MonthTerm(int month, bool looped);
	Q_INVOKABLE explicit MonthTerm(QObject *parent);
	static std::pair<QSharedPointer<MonthTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;
	void fixup(QDateTime &datetime) const override;

private:
	int _month;
};

class REMINDMELIBSHARED_EXPORT YearTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(int year MEMBER _year)

public:
	YearTerm(int year);
	Q_INVOKABLE explicit YearTerm(QObject *parent);
	static std::pair<QSharedPointer<YearTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;

private:
	int _year;
};

class REMINDMELIBSHARED_EXPORT SequenceTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(QMap<QString, int> sequence READ getSequence WRITE setSequence)
	; //dummy for the parser...

public:
	class InverseScope : public Scope {
	public:
		inline InverseScope() = default;
		inline InverseScope(Scope s) :
			Scope{s}
		{}
		inline InverseScope(ScopeFlag s) :
			Scope{s}
		{}

		inline friend bool operator<(InverseScope lhs, InverseScope rhs) {
			return static_cast<Scope>(lhs) > static_cast<Scope>(rhs);
		}
		inline friend bool operator<=(InverseScope lhs, InverseScope rhs) {
			return static_cast<Scope>(lhs) >= static_cast<Scope>(rhs);
		}
		inline friend bool operator>(InverseScope lhs, InverseScope rhs) {
			return static_cast<Scope>(lhs) < static_cast<Scope>(rhs);
		}
		inline friend bool operator>=(InverseScope lhs, InverseScope rhs) {
			return static_cast<Scope>(lhs) <= static_cast<Scope>(rhs);
		}
	};

	using Sequence = QMap<InverseScope, int>;

	SequenceTerm(Sequence &&sequence, bool looped);
	Q_INVOKABLE explicit SequenceTerm(QObject *parent);
	static std::pair<QSharedPointer<SequenceTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;

private:
	Sequence _sequence;
	QMap<QString, int> getSequence() const;
	void setSequence(const QMap<QString, int> &sequence);
};

class REMINDMELIBSHARED_EXPORT KeywordTerm : public SubTerm
{
	Q_OBJECT
	Q_PROPERTY(int days MEMBER _days)

public:
	KeywordTerm(int days);
	Q_INVOKABLE explicit KeywordTerm(QObject *parent);
	static std::pair<QSharedPointer<KeywordTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;

private:
	int _days;

};

class REMINDMELIBSHARED_EXPORT LimiterTerm : public SubTerm
{
	Q_OBJECT
	// not stored...

public:
	LimiterTerm(bool isFrom);
	Q_INVOKABLE explicit LimiterTerm(QObject *parent);
	static std::pair<QSharedPointer<LimiterTerm>, int> parse(const QStringRef &expression);
	void apply(QDateTime &datetime, bool applyFenced) const override;

	Term limitTerm() const;
	QSharedPointer<LimiterTerm> clone(Term limitTerm) const;

private:
	friend class ::EventExpressionParser;
	Term _limitTerm;

	LimiterTerm(Type type, Term &&limitTerm);
};

// general helper methods
REMINDMELIBSHARED_EXPORT QString trWord(WordKey key, bool escape = true);
REMINDMELIBSHARED_EXPORT QStringList trList(WordKey key, bool escape = true, bool sort = true);

REMINDMELIBSHARED_EXPORT QString dateTimeFormatToRegex(QString pattern, const std::function<void(QString&)> &replacer);

}

#endif // TERMS_H

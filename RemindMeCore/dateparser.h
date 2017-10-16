#ifndef DATEPARSER_H
#define DATEPARSER_H

#include <QObject>
#include <QDateTime>

namespace ReminderTypes {

class Expression : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("polymorphic", "true")

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

	Expression(QObject *parent = nullptr);
	virtual inline ~Expression() = default;

	virtual QDateTime nextSchedule(const QDateTime &since) = 0;
};

// ------------- Basic Types -------------

class Datum : public QObject
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

	Datum(QObject *parent = nullptr);

	QDate nextDate(QDate wDate) const;

	Scope scope;
	int value;
};

class Type : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool isDatum MEMBER isDatum)
	Q_PROPERTY(Datum* datum MEMBER datum)
	Q_PROPERTY(int count MEMBER count)
	Q_PROPERTY(Expression::Span span MEMBER span)

public:
	Type(QObject *parent = nullptr);

	QDate nextDate(QDate wDate) const;

	bool isDatum;
	Datum *datum;
	int count;
	Expression::Span span;
};

class TimePoint : public QObject
{
	Q_OBJECT

	Q_PROPERTY(Mode mode MEMBER mode)
	Q_PROPERTY(QDateTime date MEMBER date)
	Q_PROPERTY(Datum* datum MEMBER datum)

public:
	enum Mode {
		InvalidMode,
		DateMode,
		DatumMode,
		YearMode
	};
	Q_ENUM(Mode)

	TimePoint(QObject *parent = nullptr);

	QDate nextDate(QDate wDate) const;

	Mode mode;
	QDateTime date;
	Datum *datum;
};

// ------------- Expressions -------------

class Conjunction : public Expression
{
	Q_OBJECT

	Q_PROPERTY(QList<Expression*> expressions MEMBER expressions)

public:
	Q_INVOKABLE Conjunction(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	QList<Expression*> expressions;
};

class TimeSpan : public Expression
{
	Q_OBJECT

	Q_PROPERTY(Span span MEMBER span)
	Q_PROPERTY(int count MEMBER count)
	Q_PROPERTY(Datum* datum MEMBER datum)
	Q_PROPERTY(QTime time MEMBER time)

public:
	Q_INVOKABLE TimeSpan(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	Span span;
	int count;
	Datum *datum;
	QTime time;
};

class Loop : public Expression
{
	Q_OBJECT

	Q_PROPERTY(Type* type MEMBER type)
	Q_PROPERTY(Datum* datum MEMBER datum)
	Q_PROPERTY(QTime time MEMBER time)
	Q_PROPERTY(TimePoint* from MEMBER from)
	Q_PROPERTY(TimePoint* until MEMBER until)

public:
	Q_INVOKABLE Loop(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	Type *type;
	Datum *datum;
	QTime time;
	TimePoint *from;
	TimePoint *until;
};

class Point : public Expression
{
	Q_OBJECT

	Q_PROPERTY(TimePoint* date MEMBER date)
	Q_PROPERTY(QTime time MEMBER time)

public:
	Q_INVOKABLE Point(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	TimePoint *date;
	QTime time;
};

}

class DateParser : public QObject
{
	Q_OBJECT

public:
	explicit DateParser(QObject *parent = nullptr);

	ReminderTypes::Expression *parse(const QString &data);

private:
	static const QString timeRegex;

	ReminderTypes::Expression *parseExpression(const QString &data, QObject *parent);
	ReminderTypes::Conjunction *tryParseConjunction(const QString &data, QObject *parent);
	ReminderTypes::TimeSpan *tryParseTimeSpan(const QString &data, QObject *parent);
	ReminderTypes::Loop *tryParseLoop(const QString &data, QObject *parent);
	ReminderTypes::Point *tryParsePoint(const QString &data, QObject *parent);

	ReminderTypes::Datum *parseDatum(const QString &data, QObject *parent);
	ReminderTypes::Type *parseType(const QString &data, QObject *parent);
	ReminderTypes::TimePoint *parseTimePoint(const QString &data, QObject *parent);

	QDate parseMonthDay(const QString &data);
	QDate parseDate(const QString &data);
	QTime parseTime(const QString &data);
	ReminderTypes::Expression::Span parseSpan(const QString &data);

	static QStringList readWeekDays();
	static QStringList readMonths();
};

#endif // DATEPARSER_H

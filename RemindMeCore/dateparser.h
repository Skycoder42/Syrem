#ifndef DATEPARSER_H
#define DATEPARSER_H

#include <QObject>
#include <QDateTime>

namespace ReminderTypes {

class Reminder : public QObject
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

	Reminder(QObject *parent = nullptr);
	virtual inline ~Reminder() = default;

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
	Q_PROPERTY(Reminder::Span span MEMBER span)

public:
	Type(QObject *parent = nullptr);

	QDate nextDate(QDate wDate) const;

	bool isDatum;
	Datum *datum;
	Reminder::Span span;
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

class Conjunction : public Reminder
{
	Q_OBJECT

	Q_PROPERTY(QList<Reminder*> reminders MEMBER reminders)

public:
	Q_INVOKABLE Conjunction(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	QList<Reminder*> reminders;
};

class TimeSpan : public Reminder
{
	Q_OBJECT

	Q_PROPERTY(Span span MEMBER span)
	Q_PROPERTY(Datum* datum MEMBER datum)
	Q_PROPERTY(QTime time MEMBER time)

public:
	Q_INVOKABLE TimeSpan(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	Span span;
	Datum *datum;
	QTime time;
};

class Loop : public Reminder
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

class Point : public Reminder
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
};

#endif // DATEPARSER_H

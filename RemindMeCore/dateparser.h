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
	Reminder(QObject *parent = nullptr);
	virtual inline ~Reminder() = default;

	virtual QDateTime nextSchedule(const QDateTime &since) = 0;
};

// ------------- Basic Types -------------

class Datum : public Reminder
{
	Q_OBJECT

	Q_PROPERTY(Scope scope MEMBER scope)
	Q_PROPERTY(int value MEMBER value)
	Q_PROPERTY(QTime time MEMBER time)

public:
	enum Scope {
		WeekDayScope,
		DayScope,
		MonthScope,
		MonthDayScope
	};
	Q_ENUM(Scope)

	Q_INVOKABLE Datum(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	Scope scope;
	int value;
	QTime time;

protected:
	QDateTime prepareDate(QDateTime since);
	QDate nextDate(QDate wDate);
};

class Type : public Datum
{
	Q_OBJECT

	Q_PROPERTY(DateType dateType MEMBER dateType)

public:
	enum DateType {
		Day,
		Week,
		Month,
		Year,
		DatumType
	};
	Q_ENUM(DateType)

	Q_INVOKABLE Type(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	DateType dateType;
};

// ------------- Timepoints -------------

class TimePoint : public Reminder
{
	Q_OBJECT

public:
	Q_INVOKABLE TimePoint(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;
};

class DatePoint : public TimePoint
{
	Q_OBJECT

	Q_PROPERTY(QDateTime date MEMBER date)

public:
	Q_INVOKABLE DatePoint(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	QDateTime date;
};

class DatumPoint : public TimePoint
{
	Q_OBJECT

	Q_PROPERTY(Datum* datum MEMBER datum)

public:
	Q_INVOKABLE DatumPoint(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	Datum* datum;
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

class Offset : public Reminder
{
	Q_OBJECT

	Q_PROPERTY(Type* datum MEMBER datum)
	Q_PROPERTY(Datum* timepoint MEMBER timepoint)

public:
	Q_INVOKABLE Offset(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	Type *datum;
	Datum *timepoint;
};

class Loop : public Reminder
{
	Q_OBJECT

	Q_PROPERTY(Type* datum MEMBER datum)
	Q_PROPERTY(Datum* timepoint MEMBER timepoint)
	Q_PROPERTY(TimePoint* from MEMBER from)
	Q_PROPERTY(TimePoint* until MEMBER until)

public:
	Q_INVOKABLE Loop(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	Type *datum;
	Datum *timepoint;
	TimePoint *from;
	TimePoint *until;
};

class Range : public Reminder
{
	Q_OBJECT

	Q_PROPERTY(TimePoint* from MEMBER from)
	Q_PROPERTY(TimePoint* until MEMBER until)

public:
	Q_INVOKABLE Range(QObject *parent = nullptr);
	QDateTime nextSchedule(const QDateTime &since) override;

	TimePoint *from;
	TimePoint *until;
};

}

class DateParser : public QObject
{
	Q_OBJECT

public:
	explicit DateParser(QObject *parent = nullptr);
};

#endif // DATEPARSER_H

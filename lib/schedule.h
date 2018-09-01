#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QDateTime>
#include <QObject>

#include "libsyrem_global.h"
#include "eventexpressionparser.h"

namespace ParserTypes {
class Loop;
class Type;
class Datum;
}

class LIB_SYREM_EXPORT Schedule : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("polymorphic", "true")

	Q_PROPERTY(bool repeating READ isRepeating STORED false CONSTANT)
	Q_PROPERTY(QDateTime current READ current MEMBER _current NOTIFY currentChanged)

public:
	explicit Schedule(QObject *parent = nullptr);
	explicit Schedule(QDateTime since, QObject *parent = nullptr);

	virtual bool isRepeating() const = 0;
	QDateTime current() const;

public slots:
	QDateTime nextSchedule();

signals:
	void currentChanged(QDateTime current);

protected:
	virtual QDateTime generateNextSchedule() = 0;

private:
	QDateTime _current;
};

class LIB_SYREM_EXPORT SingularSchedule : public Schedule
{
	Q_OBJECT

public:
	Q_INVOKABLE SingularSchedule(QObject *parent = nullptr);
	SingularSchedule(QDateTime timepoint, QObject *parent = nullptr);

	bool isRepeating() const override;

protected:
	QDateTime generateNextSchedule() override;
};

class LIB_SYREM_EXPORT RepeatedSchedule : public Schedule
{
	Q_OBJECT

	Q_PROPERTY(Expressions::Term loopTerm MEMBER loopTerm)
	Q_PROPERTY(Expressions::Term fenceTerm MEMBER fenceTerm)
	Q_PROPERTY(QDateTime until MEMBER until)
	Q_PROPERTY(QDateTime fenceEnd MEMBER fenceEnd)

public:
	Q_INVOKABLE RepeatedSchedule(QObject *parent = nullptr);
	RepeatedSchedule(Expressions::Term loopTerm,
					 Expressions::Term fenceTerm,
					 QDateTime from,
					 QDateTime until,
					 QObject *parent = nullptr);

	bool isRepeating() const override;

protected:
	QDateTime generateNextSchedule() override;

private:
	Expressions::Term loopTerm;
	Expressions::Term fenceTerm;
	QDateTime until;
	QDateTime fenceEnd;

	QDateTime generateFences(const QDateTime &current);
};

class LIB_SYREM_EXPORT MultiSchedule : public Schedule
{
	Q_OBJECT

	Q_PROPERTY(QList<QSharedPointer<Schedule>> subSchedules MEMBER subSchedules)

public:
	Q_INVOKABLE MultiSchedule(QObject *parent = nullptr);
	MultiSchedule(QDateTime since, QObject *parent = nullptr);

	void addSubSchedule(const QSharedPointer<Schedule> &schedule);

	bool isRepeating() const override;

protected:
	QDateTime generateNextSchedule() override;

private:
	QList<QSharedPointer<Schedule>> subSchedules;
};

// historic schedules MAJOR convert and remove

class LIB_SYREM_EXPORT OneTimeSchedule : public Schedule
{
	Q_OBJECT

	Q_PROPERTY(QDateTime timepoint MEMBER timepoint)

public:
	Q_INVOKABLE OneTimeSchedule(QObject *parent = nullptr);
	OneTimeSchedule(QDateTime timepoint, QDateTime since, QObject *parent = nullptr);

	bool isRepeating() const override;

protected:
	QDateTime generateNextSchedule() override;

private:
	QDateTime timepoint;
};

class LIB_SYREM_EXPORT LoopSchedule : public Schedule
{
	Q_OBJECT
	friend class ParserTypes::Loop;

	Q_PROPERTY(ParserTypes::Type* type MEMBER type)
	Q_PROPERTY(ParserTypes::Datum* datum MEMBER datum)
	Q_PROPERTY(QTime time MEMBER time)

	Q_PROPERTY(QDateTime from MEMBER from)
	Q_PROPERTY(QDateTime until MEMBER until)

public:
	enum Mode {
		WeekdayMode,
		DatumMode,
		SpanMode
	};
	Q_ENUM(Mode)

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

	Q_INVOKABLE LoopSchedule(QObject *parent = nullptr);
	LoopSchedule(QDateTime since, QObject *parent = nullptr);

	bool isRepeating() const override;

protected:
	QDateTime generateNextSchedule() override;

private:
	ParserTypes::Type *type = nullptr;
	ParserTypes::Datum *datum = nullptr;
	QTime time;

	QDateTime from;
	QDateTime until;
};

Q_DECLARE_METATYPE(Schedule*)
Q_DECLARE_METATYPE(SingularSchedule*)
Q_DECLARE_METATYPE(RepeatedSchedule*)
Q_DECLARE_METATYPE(MultiSchedule*)
// old types
Q_DECLARE_METATYPE(OneTimeSchedule*)
Q_DECLARE_METATYPE(LoopSchedule*)

#endif // SCHEDULE_H

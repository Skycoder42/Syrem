#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QDateTime>
#include <QObject>

namespace ParserTypes {
class Loop;
class Type;
class Datum;
}

class Schedule : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("polymorphic", "true")

	Q_PROPERTY(bool repeating READ isRepeating STORED false CONSTANT)

public:
	explicit Schedule(QObject *parent = nullptr);

	virtual bool isRepeating() const = 0;
	virtual QDateTime nextSchedule(const QDateTime &since) = 0;//TODO important: for loops always use the last trigger as since
};

class OneTimeSchedule : public Schedule
{
	Q_OBJECT

	Q_PROPERTY(QDateTime timepoint MEMBER timepoint)

public:
	Q_INVOKABLE OneTimeSchedule(QObject *parent = nullptr);
	OneTimeSchedule(const QDateTime timepoint, QObject *parent = nullptr);

	bool isRepeating() const override;
	QDateTime nextSchedule(const QDateTime &since) override;

private:
	QDateTime timepoint;
};

class LoopSchedule : public Schedule
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

	bool isRepeating() const override;
	QDateTime nextSchedule(const QDateTime &since) override;

private:
	ParserTypes::Type *type;
	ParserTypes::Datum *datum;
	QTime time;

	QDateTime from;
	QDateTime until;
};

class MultiSchedule : public Schedule
{
	Q_OBJECT

	Q_PROPERTY(QList<Schedule*> subSchedules MEMBER subSchedules)

public:
	Q_INVOKABLE MultiSchedule(QObject *parent = nullptr);

	void addSubSchedule(Schedule *schedule);

	bool isRepeating() const override;
	QDateTime nextSchedule(const QDateTime &since) override;

private:
	QList<Schedule*> subSchedules;
};

#endif // SCHEDULE_H

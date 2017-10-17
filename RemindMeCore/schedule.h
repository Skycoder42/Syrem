#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QDateTime>
#include <QObject>

class Schedule : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("polymorphic", "true")

	Q_PROPERTY(bool repeating READ isRepeating STORED false CONSTANT)

public:
	explicit Schedule(QObject *parent = nullptr);

	virtual bool isRepeating() const = 0;
	virtual QDateTime nextSchedule(const QDateTime &since) = 0;
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

public:
	Q_INVOKABLE LoopSchedule(QObject *parent = nullptr);

	bool isRepeating() const override;
	QDateTime nextSchedule(const QDateTime &since) override;
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

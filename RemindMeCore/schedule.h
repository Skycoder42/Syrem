#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QDateTime>
#include <QObject>

class Schedule : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("polymorphic", "true")

	Q_PROPERTY(bool loop READ isLoop STORED false CONSTANT)

public:
	explicit Schedule(QObject *parent = nullptr);

	virtual bool isLoop();
	virtual QDateTime nextSchedule(const QDateTime &since) = 0;
};

class OneTimeSchedule : public Schedule
{
	Q_OBJECT

	Q_PROPERTY(QDateTime timepoint MEMBER timepoint)

public:
	Q_INVOKABLE OneTimeSchedule(QObject *parent = nullptr);
	OneTimeSchedule(const QDateTime timepoint, QObject *parent = nullptr);

	QDateTime nextSchedule(const QDateTime &since) override;

private:
	QDateTime timepoint;
};

class LoopSchedule : public Schedule
{
	Q_OBJECT

public:
	Q_INVOKABLE LoopSchedule(QObject *parent = nullptr);

	bool isLoop() override;
	QDateTime nextSchedule(const QDateTime &since) override;
};

#endif // SCHEDULE_H

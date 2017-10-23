#ifndef REMINDER_H
#define REMINDER_H

#include <QDateTime>
#include <QObject>
#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QUuid>
#include <QtDataSync/AsyncDataStore>

#include "schedule.h"

class ReminderData;

class Reminder
{
	Q_GADGET

	Q_PROPERTY(QUuid id READ id WRITE setId USER true)
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(bool important READ isImportant WRITE setImportant)

	Q_PROPERTY(QDateTime current READ current STORED false)
	Q_PROPERTY(QSharedPointer<Schedule> schedule READ getSchedule WRITE setSchedule)

public:
	Reminder();
	Reminder(const Reminder &rhs);
	Reminder &operator=(const Reminder &rhs);
	~Reminder();

	QUuid id() const;
	QString text() const;
	bool isImportant() const;
	QDateTime current() const;
	QSharedPointer<const Schedule> schedule() const;

	QtDataSync::GenericTask<void> nextSchedule(QtDataSync::AsyncDataStore *store, const QDateTime &current);

public slots:
	void setId(QUuid id);
	void setText(QString text);
	void setImportant(bool important);
	void setSchedule(QSharedPointer<Schedule> schedule);
	void setSchedule(Schedule *schedule);

private:
	QSharedDataPointer<ReminderData> _data;

	QSharedPointer<Schedule> getSchedule() const;
};

Q_DECLARE_METATYPE(Reminder)

#endif // REMINDER_H
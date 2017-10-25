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
	Q_PROPERTY(int versionCode READ versionCode)
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(bool important READ isImportant WRITE setImportant)

	Q_PROPERTY(QDateTime current READ current STORED false)
	Q_PROPERTY(bool repeating READ isRepeating STORED false)
	Q_PROPERTY(QSharedPointer<Schedule> schedule READ getSchedule WRITE setSchedule)

	Q_PROPERTY(QDateTime snooze READ snooze WRITE setSnooze)

public:
	Reminder();
	Reminder(const Reminder &rhs);
	Reminder &operator=(const Reminder &rhs);
	~Reminder();

	QUuid id() const;
	int versionCode() const;
	QString text() const;
	bool isImportant() const;
	QDateTime current() const;
	bool isRepeating() const;
	QSharedPointer<const Schedule> schedule() const;
	QDateTime snooze() const;

	QtDataSync::GenericTask<void> nextSchedule(QtDataSync::AsyncDataStore *store, const QDateTime &current);
	QtDataSync::GenericTask<void> performSnooze(QtDataSync::AsyncDataStore *store, const QDateTime &snooze);

public slots:
	void setId(QUuid id);
	void setText(QString text);
	void setImportant(bool important);
	void setSchedule(QSharedPointer<Schedule> schedule);
	void setSchedule(Schedule *schedule);

private:
	QSharedDataPointer<ReminderData> _data;

	QSharedPointer<Schedule> getSchedule() const;
	void setSnooze(QDateTime snooze);
};

Q_DECLARE_METATYPE(Reminder)

#endif // REMINDER_H

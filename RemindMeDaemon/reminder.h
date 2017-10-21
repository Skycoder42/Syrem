#ifndef REMINDER_H
#define REMINDER_H

#include <QDateTime>
#include <QObject>
#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QUuid>

#include "schedule.h"

class ReminderData;

class Reminder
{
	Q_GADGET

	Q_PROPERTY(QUuid id READ id WRITE setId USER true)
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(bool important READ isImportant WRITE setImportant)

	Q_PROPERTY(QDateTime current READ current STORED false)

public:
	Reminder();
	Reminder(const Reminder &rhs);
	Reminder &operator=(const Reminder &rhs);
	~Reminder();

	QUuid id() const;
	QString text() const;
	bool isImportant() const;
	QDateTime current() const;

public slots:
	void setId(QUuid id);
	void setText(QString text);
	void setImportant(bool important);

private:
	QSharedDataPointer<ReminderData> _data;
};

#endif // REMINDER_H

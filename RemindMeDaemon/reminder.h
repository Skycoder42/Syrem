#ifndef REMINDER_H
#define REMINDER_H

#include <QDateTime>
#include <QObject>
#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QUuid>

class ReminderData;

class ReminderRule : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("polymorphic", "true")

public:
	ReminderRule(QObject *parent = nullptr);
	virtual inline ~ReminderRule() = default;

	virtual bool trySetup(const QString &data) = 0;
	virtual QDateTime nextSchedule(const QDateTime &since) const = 0;
};

class Reminder
{
	Q_GADGET

	Q_PROPERTY(QUuid id READ id WRITE setId)
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(bool important READ isImportant WRITE setImportant)
	Q_PROPERTY(QDateTime nextSchedule READ nextSchedule WRITE setNextSchedule)

	Q_PROPERTY(QSharedPointer<const ReminderRule> rule READ rule WRITE setRule)

public:
	Reminder();
	Reminder(const Reminder &rhs);
	Reminder &operator=(const Reminder &rhs);
	~Reminder();

	QUuid id() const;
	QString text() const;
	bool isImportant() const;
	QDateTime nextSchedule() const;
	QSharedPointer<const ReminderRule> rule() const;

public slots:
	void setId(QUuid id);
	void setText(QString text);
	void setImportant(bool important);
	void setNextSchedule(QDateTime nextSchedule);
	void setRule(QSharedPointer<const ReminderRule> rule);

private:
	QSharedDataPointer<ReminderData> _data;
};

#endif // REMINDER_H

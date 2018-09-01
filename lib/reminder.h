#ifndef REMINDER_H
#define REMINDER_H

#include <QDateTime>
#include <QObject>
#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QUrl>
#include <QUuid>
#include <QtDataSync/DataTypeStore>

#include "libsyrem_global.h"
#include "schedule.h"

class ReminderData;

class LIB_SYREM_EXPORT Reminder
{
	Q_GADGET

	Q_PROPERTY(QUuid id READ id WRITE setId USER true)
	Q_PROPERTY(QUuid bugId READ id WRITE setId STORED false) //NOTE remove again...
	Q_PROPERTY(quint32 versionCode READ versionCode WRITE setVersionCode)
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(bool important READ isImportant WRITE setImportant)

	Q_PROPERTY(QString htmlDescription READ htmlDescription STORED false)
	Q_PROPERTY(bool hasUrls READ hasUrls STORED false)

	Q_PROPERTY(QDateTime current READ current STORED false)
	Q_PROPERTY(bool repeating READ isRepeating STORED false)
	Q_PROPERTY(State triggerState READ triggerState STORED false)
	Q_PROPERTY(QSharedPointer<Schedule> schedule READ getSchedule WRITE setSchedule)

	Q_PROPERTY(QDateTime snooze READ snooze WRITE setSnooze)

	Q_PROPERTY(QString expression READ expression WRITE setExpression)

public:
	enum State {
		Normal,
		NormalRepeating,
		Snoozed,
		Triggered
	};
	Q_ENUM(State)

	Reminder();
	Reminder(const Reminder &rhs);
	Reminder(Reminder &&rhs) noexcept;
	Reminder &operator=(const Reminder &rhs);
	Reminder &operator=(Reminder &&rhs) noexcept;
	~Reminder();

	QUuid id() const;
	quint32 versionCode() const;

	QString description() const;
	bool isImportant() const;
	QDateTime current() const;
	bool isRepeating() const;
	State triggerState() const;
	QSharedPointer<const Schedule> schedule() const;
	QDateTime snooze() const;
	QString expression() const;

	QList<QUrl> extractUrls() const;
	QString htmlDescription() const;
	bool hasUrls() const;

	void nextSchedule(QtDataSync::DataStore *store, const QDateTime &current);
	void performSnooze(QtDataSync::DataStore *store, const QDateTime &snooze);

	Q_INVOKABLE void openUrls() const;

	void setId(QUuid id);
	void setDescription(QString description);
	void setImportant(bool important);
	void setSchedule(QSharedPointer<Schedule> schedule);
	void setExpression(QString expression);

private:
	QSharedDataPointer<ReminderData> _data;
	mutable struct {
		bool set = false;
		QList<QUrl> urls;
	} _urlCache;

	void setVersionCode(quint32 versionCode);
	QSharedPointer<Schedule> getSchedule() const;
	void setSnooze(QDateTime snooze);
};

Q_DECLARE_METATYPE(Reminder)

#endif // REMINDER_H

#include "reminder.h"

#include <QRegularExpression>

using namespace QtDataSync;

class ReminderData : public QSharedData
{
public:
	ReminderData();
	ReminderData(const ReminderData &other);

	QUuid id;
	quint32 versionCode;
	QString text;
	bool important;
	QSharedPointer<Schedule> schedule;
	QDateTime snooze;
};

Reminder::Reminder() :
	_data(new ReminderData)
{}

Reminder::Reminder(const Reminder &rhs) :
	_data(rhs._data)
{}

Reminder &Reminder::operator=(const Reminder &rhs)
{
	if (this != &rhs)
		_data.operator=(rhs._data);
	return *this;
}

Reminder::~Reminder() = default;

QUuid Reminder::id() const
{
	return _data->id;
}

quint32 Reminder::versionCode() const
{
	return _data->versionCode;
}

QString Reminder::description() const
{
	return _data->text;
}

bool Reminder::isImportant() const
{
	return _data->important;
}

QDateTime Reminder::current() const
{
	if(_data->schedule) {
		if(_data->snooze.isValid())
			return _data->snooze;
		else
			return _data->schedule->current();
	} else
		return {};
}

bool Reminder::isRepeating() const
{
	if(_data->schedule)
		return _data->schedule->isRepeating();
	else
		return false;
}

Reminder::State Reminder::triggerState() const
{
	if(current() <= QDateTime::currentDateTime())
		return Triggered;
	else if(snooze().isValid())
		return Snoozed;
	else if(isRepeating())
		return NormalRepeating;
	else
		return Normal;
}

QSharedPointer<const Schedule> Reminder::schedule() const
{
	return _data->schedule.constCast<const Schedule>();
}

QDateTime Reminder::snooze() const
{
	return _data->snooze;
}

void Reminder::nextSchedule(DataStore *store, const QDateTime &current)
{
	Q_ASSERT_X(_data->schedule, Q_FUNC_INFO, "cannot call next schedule without an assigned schedule");

	QDateTime res;
	do {
		res = _data->schedule->nextSchedule();
	} while(res.isValid() && res <= current);

	_data->snooze = QDateTime();//reset any snoozes
	_data->versionCode++;
	if(res.isValid())
		store->save(*this);
	else
		store->remove<Reminder>(_data->id);
}

void Reminder::performSnooze(DataStore *store, const QDateTime &snooze)
{
	_data->snooze = snooze;
	_data->versionCode++;
	store->save(*this);
}

void Reminder::setId(const QUuid &id)
{
	_data->id = id;
}

void Reminder::setDescription(const QString &text)
{
	_data->text = text;
}

void Reminder::setImportant(bool important)
{
	_data->important = important;
}

void Reminder::setSchedule(const QSharedPointer<Schedule> &schedule)
{
	_data->schedule = schedule;
}

void Reminder::setSchedule(Schedule *schedule)
{
	_data->schedule = QSharedPointer<Schedule>(schedule);
}

void Reminder::setVersionCode(quint32 versionCode)
{
	_data->versionCode = versionCode;
}

QSharedPointer<Schedule> Reminder::getSchedule() const
{
	return _data->schedule;
}

void Reminder::setSnooze(QDateTime snooze)
{
	_data->snooze = snooze;
}



ReminderData::ReminderData() :
	QSharedData(),
	id(QUuid::createUuid()),
	versionCode(1),
	text(),
	important(false),
	schedule(nullptr),
	snooze()
{}

ReminderData::ReminderData(const ReminderData &other):
	QSharedData(other),
	id(other.id),
	versionCode(other.versionCode),
	text(other.text),
	important(other.important),
	schedule(other.schedule),
	snooze(other.snooze)
{}

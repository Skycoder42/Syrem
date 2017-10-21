#include "reminder.h"

using namespace QtDataSync;

class ReminderData : public QSharedData
{
public:
	ReminderData();
	ReminderData(const ReminderData &other);

	QUuid id;
	QString text;
	bool important;
	QSharedPointer<Schedule> schedule;
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

QString Reminder::text() const
{
	return _data->text;
}

bool Reminder::isImportant() const
{
	return _data->important;
}

QDateTime Reminder::current() const
{
	if(_data->schedule)
		return _data->schedule->current();
	else
		return {};
}

QSharedPointer<const Schedule> Reminder::schedule() const
{
	return _data->schedule.constCast<const Schedule>();
}

QtDataSync::GenericTask<void> Reminder::nextSchedule(AsyncDataStore *store)
{
	Q_ASSERT_X(_data->schedule, Q_FUNC_INFO, "cannot call next schedule without an assigned schedule");
	auto res = _data->schedule->nextSchedule();
	if(res.isValid())
		return store->save(*this);
	else
		return store->remove<Reminder>(_data->id).toGeneric<void>();
}

void Reminder::setId(QUuid id)
{
	_data->id = id;
}

void Reminder::setText(QString text)
{
	_data->text = text;
}

void Reminder::setImportant(bool important)
{
	_data->important = important;
}

void Reminder::setSchedule(QSharedPointer<Schedule> schedule)
{
	_data->schedule = schedule;
}

void Reminder::setSchedule(Schedule *schedule)
{
	_data->schedule = QSharedPointer<Schedule>(schedule);
}

QSharedPointer<Schedule> Reminder::getSchedule() const
{
	return _data->schedule;
}



ReminderData::ReminderData() :
	QSharedData(),
	id(QUuid::createUuid()),
	text(),
	important(false)
{}

ReminderData::ReminderData(const ReminderData &other):
	QSharedData(other),
	id(other.id),
	text(other.text),
	important(other.important)
{}

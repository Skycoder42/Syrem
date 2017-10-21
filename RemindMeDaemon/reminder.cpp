#include "reminder.h"

class ReminderData : public QSharedData
{
public:
	ReminderData();
	ReminderData(const ReminderData &other);

	QUuid id;
	QString text;
	bool important;
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
	return QDateTime::currentDateTime(); //TODO proper implement
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

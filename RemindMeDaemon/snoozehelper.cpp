#include "snoozehelper.h"

SnoozeHelper::SnoozeHelper(QObject *parent) :
	SnoozeHelperSimpleSource(parent)
{}

void SnoozeHelper::loadReminder(const QUuid &id)
{
}

void SnoozeHelper::snoozeReminder(const QUuid &id, const QString &expression)
{
}

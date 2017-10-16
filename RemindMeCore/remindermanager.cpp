#include "remindermanager.h"

ReminderManager::ReminderManager(QObject *parent) :
	ReminderManagerSimpleSource(parent)
{
	setReady(true);
}

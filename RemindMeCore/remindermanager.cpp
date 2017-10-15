#include "remindermanager.h"

ReminderManager::ReminderManager(QObject *parent) :
	ReminderManagerSimpleSource(parent)
{}

void ReminderManager::server_slot(bool clientState)
{
	Q_UNUSED(clientState)
}

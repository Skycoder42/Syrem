#ifndef REMINDERMANAGER_H
#define REMINDERMANAGER_H

#include <QObject>
#include "rep_remindermanager_source.h"

class ReminderManager : public ReminderManagerSimpleSource
{
	Q_OBJECT

public:
	explicit ReminderManager(QObject *parent = nullptr);
};

#endif // REMINDERMANAGER_H

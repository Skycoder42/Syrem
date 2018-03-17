#ifndef REMINDMELIB_H
#define REMINDMELIB_H

#include <QtDataSync/Setup>
#include <QtDataSync/DataTypeStore>

#include "remindmelib_global.h"
#include "reminder.h"

typedef QtDataSync::DataTypeStore<Reminder, QUuid> ReminderStore;

namespace RemindMe {

REMINDMELIBSHARED_EXPORT void setup(QtDataSync::Setup &setup);
REMINDMELIBSHARED_EXPORT QString whenExpressionHelp();

}

#endif // REMINDMELIB_H

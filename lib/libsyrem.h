#ifndef LIBSYREM_H
#define LIBSYREM_H

#include <QtDataSync/Setup>
#include <QtDataSync/DataTypeStore>

#include "libsyrem_global.h"
#include "reminder.h"

using ReminderStore = QtDataSync::DataTypeStore<Reminder, QUuid>;

namespace Syrem {

LIB_SYREM_EXPORT void prepareTranslations(const QString &tsName);
LIB_SYREM_EXPORT void setup(QtDataSync::Setup &setup);
LIB_SYREM_EXPORT QString whenExpressionHelp();

}

#endif // LIBSYREM_H

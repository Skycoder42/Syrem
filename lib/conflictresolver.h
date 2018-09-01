#ifndef CONFLICTRESOLVER_H
#define CONFLICTRESOLVER_H

#include <QObject>
#include <QtDataSync/ConflictResolver>

#include "reminder.h"

class ConflictResolver : public QtDataSync::GenericConflictResolver<Reminder>
{
	Q_OBJECT

public:
	explicit ConflictResolver(QObject *parent = nullptr);

	Reminder resolveConflict(Reminder data1, Reminder data2, QObject *parent) const override;
};

#endif // CONFLICTRESOLVER_H

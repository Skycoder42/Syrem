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

	QJsonObject resolveConflict(int typeId, const QJsonObject &data1, const QJsonObject &data2) const override;
	Reminder resolveConflict(Reminder data1, Reminder data2, QObject *parent) const override;
};

#endif // CONFLICTRESOLVER_H

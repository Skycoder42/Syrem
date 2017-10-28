#ifndef SNOOZEHELPER_H
#define SNOOZEHELPER_H

#include <QObject>
#include "rep_snoozehelper_source.h"

class SnoozeHelper : public SnoozeHelperSimpleSource
{
	Q_OBJECT

public:
	explicit SnoozeHelper(QObject *parent = nullptr);

public slots:
	void loadReminder(const QUuid &id) override;
	void snoozeReminder(const QUuid &id, const QString &expression) override;
};

#endif // SNOOZEHELPER_H

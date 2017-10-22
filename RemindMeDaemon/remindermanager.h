#ifndef REMINDERMANAGER_H
#define REMINDERMANAGER_H

#include <QObject>
#include <QtDataSync/AsyncDataStore>
#include "dateparser.h"
#include "reminder.h"
#include "rep_remindermanager_source.h"
#include "scheduler.h"

class ReminderManager : public ReminderManagerSimpleSource
{
	Q_OBJECT

public:
	explicit ReminderManager(QObject *parent = nullptr);

public slots:
	void createReminder(const QString &text, bool important, const QString &expression) override;

private slots:
	void scheduleTriggered(const QUuid &id);
	void dataChanged(int metaTypeId, const QString &key, bool wasDeleted);

private:
	QtDataSync::AsyncDataStore *_store;
	DateParser *_parser;
	Scheduler *_scheduler;
};

#endif // REMINDERMANAGER_H

#ifndef REMINDERMANAGER_H
#define REMINDERMANAGER_H

#include <QObject>
#include <QSettings>
#include <QtDataSync/DataTypeStore>
#include "dateparser.h"
#include "reminder.h"
#include "rep_remindermanager_source.h"
#include "ischeduler.h"

class ReminderManager : public ReminderManagerSimpleSource
{
	Q_OBJECT

public:
	explicit ReminderManager(QObject *parent = nullptr);

public slots:
	void createReminder(const QString &text, bool important, const QString &expression) override;
	void removeReminder(const QUuid &id) override;

private:
	QtDataSync::DataTypeStore<Reminder, QUuid> *_store;
	QSettings *_settings;
	DateParser *_parser;
	IScheduler *_scheduler;
};

#endif // REMINDERMANAGER_H

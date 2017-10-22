#ifndef REMINDERMANAGER_H
#define REMINDERMANAGER_H

#include <QObject>
#include <QtDataSync/AsyncDataStore>
#include "dateparser.h"
#include "rep_remindermanager_source.h"

class ReminderManager : public ReminderManagerSimpleSource
{
	Q_OBJECT

public:
	explicit ReminderManager(QObject *parent = nullptr);

public slots:
	void createReminder(const QString &text, bool important, const QString &expression) override;

private:
	QtDataSync::AsyncDataStore *_store;
	DateParser *_parser;
};

#endif // REMINDERMANAGER_H

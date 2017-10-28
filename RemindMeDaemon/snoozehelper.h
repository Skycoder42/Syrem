#ifndef SNOOZEHELPER_H
#define SNOOZEHELPER_H

#include <QObject>
#include <QSettings>
#include <QtDataSync/AsyncDataStore>
#include "reminder.h"
#include "rep_snoozehelper_source.h"
#include "dateparser.h"

class SnoozeHelper : public SnoozeHelperSimpleSource
{
	Q_OBJECT

public:
	explicit SnoozeHelper(QObject *parent = nullptr);

public slots:
	void loadReminder(const QUuid &id) override;
	void snoozeReminder(const QUuid &id, const QString &expression) override;

private:
	QtDataSync::AsyncDataStore *_store;
	DateParser *_parser;
	QSettings *_settings;

	QHash<QUuid, Reminder> _remCache;
};

#endif // SNOOZEHELPER_H

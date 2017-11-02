#include "reminder.h"
#include "snoozehelper.h"
using namespace QtDataSync;

SnoozeHelper::SnoozeHelper(QObject *parent) :
	SnoozeHelperSimpleSource(parent),
	_store(new AsyncDataStore(this)),
	_parser(new DateParser(this)),
	_settings(new QSettings(this)),
	_remCache()
{
	_settings->beginGroup(QStringLiteral("daemon"));
}

void SnoozeHelper::loadReminder(const QUuid &id, quint32 versionCode)
{
	_store->load<Reminder>(id).onResult(this, [this, versionCode](Reminder reminder) {
		if(reminder.versionCode() == versionCode) {
			_remCache.insert(reminder.id(), reminder);
			emit reminderLoaded(reminder.id(), reminder.description());
		} else
			emit reminderError(reminder.id(), tr("Reminder has already been snoozed!"), true);
	}, [this, id](const QException &e) {
		qCritical() << "Failed to load reminder with error:" << e.what();
		emit reminderError(id, tr("Failed to load reminder!"), true);
	});
}

void SnoozeHelper::snoozeReminder(const QUuid &id, const QString &expression)
{
	try {
		auto nextTime = _parser->snoozeParse(expression);
		auto rem = _remCache.take(id);
		rem.performSnooze(_store, nextTime);
		emit reminderSnoozed(id);
	} catch (QString &s) {
		emit reminderError(id, s, false);
	}
}

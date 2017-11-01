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
	auto expre = _parser->parse(expression);
	if(!expre) {
		emit reminderError(id,
						   tr("The entered text is not a valid expression. Error message:\n%1").arg(_parser->lastError()),
						   false);
		return;
	}

	auto schedule = expre->createSchedule(QDateTime::currentDateTime(),
										  _settings->value(QStringLiteral("defaultTime"), QTime(9,0)).toTime(),
										  this);
	if(!schedule) {
		emit reminderError(id, tr("Given expression is valid, but evaluates to a timepoint in the past!"), false);
		return;
	}

	if(schedule->isRepeating()) {
		emit reminderError(id, tr("Given expression evaluates to more the 1 timepoint!"), false);
		return;
	}

	auto nextTime = schedule->nextSchedule();
	if(!nextTime.isValid()) {
		emit reminderError(id, tr("Given expression is valid, but evaluates to a timepoint in the past!"), false);
		return;
	}

	auto rem = _remCache.take(id);
	rem.performSnooze(_store, nextTime);
	emit reminderSnoozed(id);
}

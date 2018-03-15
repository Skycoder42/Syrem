#include "registry.h"
#include "remindermanager.h"

using namespace QtDataSync;

ReminderManager::ReminderManager(QObject *parent) :
	ReminderManagerSimpleSource(parent),
	_store(new DataTypeStore<Reminder, QUuid>(this)),
	_settings(new QSettings(this)),
	_parser(new DateParser(this)),
	_scheduler(Registry::acquire<IScheduler>())
{
	Q_ASSERT(_scheduler);
	_settings->beginGroup(QStringLiteral("daemon"));
}

void ReminderManager::createReminder(const QString &text, bool important, const QString &expression)
{
	auto expr = _parser->parse(expression);
	if(!expr) {
		emit reminderError(true, tr("<p>Invalid \"when\" expression! Error message:</p><p><i>%1</i></p>").arg(_parser->lastError()));
		return;
	}
	auto sched = expr->createSchedule(QDateTime::currentDateTime(), _settings->value(QStringLiteral("defaultTime"), QTime(9,0)).toTime());
	if(!sched) {
		emit reminderError(true, tr("Given \"when\" expression is valid, but evaluates to a timepoint in the past!"));
		return;
	}

	auto next = sched->nextSchedule();
	if(!next.isValid()) {
		emit reminderError(true, tr("Given \"when\" expression is valid, but evaluates to a timepoint in the past!"));
		return;
	}

	Reminder rem;
	rem.setDescription(text);
	rem.setImportant(important);
	rem.setSchedule(sched);

	try {
		_store->save(rem);
		emit reminderCreated();
	} catch(QException &e) {
		qCritical() << "Failed to create reminder with error:" << e.what();
		emit reminderError(true, tr("Failed to save reminder!"));
	}
}

void ReminderManager::removeReminder(const QUuid &id)
{
	_scheduler->cancleReminder(id);
	try {
		if(!_store->remove(id))
			qWarning() << "Reminder with id" << id << "has already been removed";
	} catch(QException &e) {
		qCritical() << "Failed to remove reminder with error:" << e.what();
		emit reminderError(false, tr("Failed to delete reminder!"));
	}
}

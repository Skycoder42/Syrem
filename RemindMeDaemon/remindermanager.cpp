#include "registry.h"
#include "remindermanager.h"

using namespace QtDataSync;

ReminderManager::ReminderManager(QObject *parent) :
	ReminderManagerSimpleSource(parent),
	_store(new AsyncDataStore(this)),
	_parser(new DateParser(this)),
	_scheduler(Registry::acquire<Scheduler>())
{
	connect(dynamic_cast<QObject*>(_scheduler), SIGNAL(scheduleTriggered(QUuid)),
			this, SLOT(scheduleTriggered(QUuid)),
			Qt::QueuedConnection);

	connect(_store, &AsyncDataStore::dataChanged,
			this, &ReminderManager::dataChanged);

	_store->loadAll<Reminder>().onResult([this](QList<Reminder> reminders) {
		foreach(auto rem, reminders)
			_scheduler->scheduleReminder(rem.id(), rem.current());
	}, [this](const QException &e) {
		qCritical() << "Failed to load stored reminders with error:" << e.what();
		//TODO proper error handling -> reminderError not used yet
		emit reminderError(tr("Failed to load existing reminders!"));
	});
}

void ReminderManager::createReminder(const QString &text, bool important, const QString &expression)
{
	auto expr = _parser->parse(expression);
	if(!expr) {
		emit reminderCreateError(tr("Invalid \"when\" expression!"));//TODO add error message
		return;
}
	auto sched = expr->createSchedule(QDateTime::currentDateTime());
	expr->deleteLater();
	if(!sched) {
		emit reminderCreateError(tr("Given \"when\" expression is valid, but evaluates to a timepoint in the past!"));
		return;
	}

	auto next = sched->nextSchedule();
	if(!next.isValid()) {
		emit reminderCreateError(tr("Given \"when\" expression is valid, but evaluates to a timepoint in the past!"));
		return;
	}

	Reminder rem;
	rem.setText(text);
	rem.setImportant(important);
	rem.setSchedule(sched);

	_store->save(rem).onResult(this, [this](){
		emit reminderCreated();
	}, [this](const QException &e) {
		qCritical() << "Failed to create reminder with error:" << e.what();
		emit reminderCreateError(tr("Failed to save reminder!"));
	});
}

void ReminderManager::removeReminder(const QUuid &id)
{
	_scheduler->cancleReminder(id);
	_store->remove<Reminder>(id).onResult(this, [this, id](bool removed) {
		if(!removed)
			qWarning() << "Reminder with id" << id << "has already been removed";
	}, [this](const QException &e) {
		qCritical() << "Failed to load reminder with error:" << e.what();
		emit reminderError(tr("Failed to delete reminder!"));
	});
}

void ReminderManager::scheduleTriggered(const QUuid &id)
{
	qDebug() << "triggered!" << id << "at" << QDateTime::currentDateTime().toString(Qt::TextDate);
}

void ReminderManager::dataChanged(int metaTypeId, const QString &key, bool wasDeleted)
{
	if(metaTypeId == qMetaTypeId<Reminder>()) {
		if(wasDeleted)
			_scheduler->cancleReminder(QUuid(key));
		else {
			_store->load<Reminder>(key).onResult(this, [this](Reminder rem) {
				_scheduler->scheduleReminder(rem.id(), rem.current());
			}, [this](const QException &e) {
				qCritical() << "Failed to load reminder with error:" << e.what();
				emit reminderError(tr("Failed to schedule reminder!"));
			});
		}
	}
}

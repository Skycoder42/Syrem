#include "reminder.h"
#include "remindermanager.h"

using namespace QtDataSync;

ReminderManager::ReminderManager(QObject *parent) :
	ReminderManagerSimpleSource(parent),
	_store(new AsyncDataStore(this)),
	_parser(new DateParser(this))
{}

void ReminderManager::createReminder(QString text, bool important, QString expression)
{
	auto expr = _parser->parse(expression);
	if(!expr) {
		emit reminderError(tr("Invalid \"when\" expression!"));//TODO add error message
		return;
	}
	auto sched = expr->createSchedule(QDateTime::currentDateTime());
	expr->deleteLater();
	if(!sched) {
		emit reminderError(tr("Given \"when\" expression is valid, but evaluates to a timepoint in the past!"));
		return;
	}

	auto next = sched->nextSchedule();
	if(!next.isValid()) {
		emit reminderError(tr("Given \"when\" expression is valid, but evaluates to a timepoint in the past!"));
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
		emit reminderError(tr("Failed to save reminder"));
	});
}

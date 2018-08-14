#include "snoozeviewmodel.h"
#include <QtMvvmCore/Messages>
#include <snoozetimes.h>
#include <syncedsettings.h>

const QString SnoozeViewModel::paramReminder{QStringLiteral("reminder")};

SnoozeViewModel::SnoozeViewModel(QObject *parent) :
	ViewModel{parent},
	_store{new ReminderStore{this}}
{}

QVariantHash SnoozeViewModel::showParams(const Reminder &reminder)
{
	return {
		{paramReminder, QVariant::fromValue<Reminder>(reminder)}
	};
}

bool SnoozeViewModel::isValid() const
{
	return !_reminder.id().isNull();
}

QString SnoozeViewModel::description() const
{
	return isValid() ?
				tr("Choose a snooze time for the reminder:<br/><i>%1</i>").arg(_reminder.description()) :
				tr("<i>Loading Reminder, please wait…</i>");
}

QStringList SnoozeViewModel::snoozeTimes() const
{
	return _snoozeTimes;
}

QString SnoozeViewModel::expression() const
{
	return _expression;
}

bool SnoozeViewModel::snooze()
{
	if(!isValid())
		return false;

	try {
		auto nextTime = _parser->snoozeParse(_expression);
		_reminder.performSnooze(_store->store(), nextTime);
		return true;
	} catch (DateParserException &e) {
		QtMvvm::critical(tr("Snoozing failed!"), e.qWhat());
	} catch (QException &e) {
		qCritical() << "Failed to snooze reminder with error:" << e.what();
		QtMvvm::critical(tr("Snoozing failed!"),
						 tr("Unable update reminder in data store. Snooze not performed."));
	}

	return false;
}

void SnoozeViewModel::setExpression(const QString &expression)
{
	if (_expression == expression)
		return;

	_expression = expression;
	emit expressionChanged(_expression);
}

void SnoozeViewModel::onInit(const QVariantHash &params)
{
	Q_ASSERT_X(params.contains(paramReminder), Q_FUNC_INFO, "SnoozeViewModel must always have at least the id parameter");

	_snoozeTimes = _settings->scheduler.snoozetimes;
	_reminder = params.value(paramReminder).value<Reminder>();
	emit reminderLoaded();
}

#include "snoozeviewmodel.h"
#include <QtMvvmCore/Messages>
#include <snoozetimes.h>
#include <syncedsettings.h>

const QString SnoozeViewModel::paramId = QStringLiteral("id");
const QString SnoozeViewModel::paramVersionCode = QStringLiteral("versionCode");

SnoozeViewModel::SnoozeViewModel(QObject *parent) :
	ViewModel(parent),
	_store(new ReminderStore(this)),
	_parser(new DateParser(this)),
	_reminder(),
	_snoozeTimes(),
	_expression()
{}

QVariantHash SnoozeViewModel::showParams(const QUuid &id)
{
	return {
		{paramId, id}
	};
}

QVariantHash SnoozeViewModel::showParams(const QUuid &id, quint32 versionCode)
{
	return {
		{paramId, id},
		{paramVersionCode, versionCode}
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
	Q_ASSERT_X(params.contains(paramId), Q_FUNC_INFO, "SnoozeViewModel must always have at least the id parameter");

	_snoozeTimes = SyncedSettings::instance()->scheduler.snooze.times; //TODO fix default value

	try {
		_reminder = _store->load(params.value(paramId).toUuid());
		if(params.contains(paramVersionCode) &&
		   _reminder.versionCode() != params.value(paramVersionCode).toUInt()) {
			_reminder = Reminder();
			QtMvvm::critical(tr("Snoozing failed!"),
							 tr("Reminder has already been updated - cannot be snoozed anymore."));
		} else
			emit reminderLoaded();
	} catch (QException &e) {
		qCritical() << "Failed to load reminder with error:" << e.what();
		QtMvvm::critical(tr("Snoozing failed!"),
						 tr("Unable to load the reminder that should be snoozed!"));
	}
}

#include "snoozeviewmodel.h"
#include "termselectionviewmodel.h"
#include <QtMvvmCore/Messages>
#include <snoozetimes.h>
#include <syncedsettings.h>

const QString SnoozeViewModel::paramReminder{QStringLiteral("reminder")};
const QString SnoozeViewModel::paramReminderId{QStringLiteral("id")};

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

QVariantHash SnoozeViewModel::showParams(QUuid reminderId)
{
	return {
		{paramReminderId, reminderId}
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

bool SnoozeViewModel::isBlocked() const
{
	return _blocked;
}

void SnoozeViewModel::snooze()
{
	if(!isValid())
		return;

	try {
		auto term = _parser->parseExpression(_expression);
		if(_parser->needsSelection(term)) {
			setBlocked(true);
			showForResult<TermSelectionViewModel>(TermSelectCode, TermSelectionViewModel::showParams(term));
		} else
			finishSnooze(term.first());
	} catch (EventExpressionParserException &e) {
		QtMvvm::critical(tr("Snoozing failed!"), e.message());
	}
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
	if(params.contains(paramReminderId)) {
		try {
			_reminder = _store->load(params.value(paramReminderId).toUuid());
		} catch(QException &e) {
			qCritical() << "Failed to load reminder to snooze it with error:" << e.what();
			QtMvvm::critical(tr("Snoozing failed!"),
							 tr("Unable loard reminder from data store. Unable to snooze."));
		}
	} else {
		Q_ASSERT_X(params.contains(paramReminder), Q_FUNC_INFO, "SnoozeViewModel must always have at least the id parameter");
		_reminder = params.value(paramReminder).value<Reminder>();
	}

	_snoozeTimes = _settings->scheduler.snoozetimes;
	emit reminderLoaded();
}

void SnoozeViewModel::onResult(quint32 requestCode, const QVariant &result)
{
	if(requestCode == TermSelectCode) {
		auto res = TermSelectionViewModel::readSingleResult(result);
		if(!res.isEmpty())
			finishSnooze(res);
		else
			setBlocked(false);
	}
}

void SnoozeViewModel::finishSnooze(const Expressions::Term &term)
{
	try {
		_reminder.performSnooze(_store->store(), _parser->evaluteTerm(term));
		emit close();
	} catch (EventExpressionParserException &e) {
		QtMvvm::critical(tr("Snoozing failed!"), e.message());
		setBlocked(false);
	} catch (QException &e) {
		qCritical() << "Failed to snooze reminder with error:" << e.what();
		QtMvvm::critical(tr("Snoozing failed!"),
						 tr("Unable update reminder in data store. Snooze not performed."));
		setBlocked(false);
	}
}

void SnoozeViewModel::setBlocked(bool blocked)
{
	if(blocked == _blocked)
		return;

	_blocked = blocked;
	emit blockedChanged(_blocked);
}

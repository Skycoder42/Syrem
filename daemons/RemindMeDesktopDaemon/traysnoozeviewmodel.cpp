#include "traysnoozeviewmodel.h"
#include <QtMvvmCore/Messages>
#include <termselectionviewmodel.h>

const QString TraySnoozeViewModel::paramNotifier = QStringLiteral("notifier");
const QString TraySnoozeViewModel::paramReminders = QStringLiteral("reminders");

TraySnoozeViewModel::TraySnoozeViewModel(QObject *parent) :
	ViewModel{parent}
{}

QVariantHash TraySnoozeViewModel::showParams(INotifier *notifier, const QList<Reminder> &reminders)
{
	return {
		{paramNotifier, QVariant::fromValue(notifier)},
		{paramReminders, QVariant::fromValue(reminders)}
	};
}

QList<Reminder> TraySnoozeViewModel::reminders() const
{
	return _reminders.values();
}

SyncedSettings *TraySnoozeViewModel::settings() const
{
	return _settings;
}

bool TraySnoozeViewModel::isBlocked() const
{
	return _blocked;
}

void TraySnoozeViewModel::performComplete(QUuid id)
{
	auto rem = _reminders.take(id);
	if(rem.id().isNull())
		return;

	emit _notifier->messageCompleted(id, rem.versionCode());
	emit remindersChanged(_reminders.values());
}

void TraySnoozeViewModel::performSnooze(QUuid id, const QString &expression)
{
	auto rem = _reminders.value(id);
	if(rem.id().isNull())
		return;

	try {
		auto term = _parser->parseExpression(expression);
		if(_parser->needsSelection(term)) {
			setBlocked(true);
			_requests.insert(++_requestCounter, id);
			showForResult<TermSelectionViewModel>(_requestCounter, TermSelectionViewModel::showParams(term));
		} else
			finishSnooze(id, term.first());
	} catch (EventExpressionParserException &e) {
		QtMvvm::critical(tr("Snoozing failed!"), e.message());
	}
}

void TraySnoozeViewModel::openUrls(QUuid id)
{
	auto rem = _reminders.value(id);
	if(!rem.id().isNull())
		rem.openUrls();
}

void TraySnoozeViewModel::onInit(const QVariantHash &params)
{
	_notifier = params.value(paramNotifier).value<INotifier*>();
	for(const auto &reminder : params.value(paramReminders).value<QList<Reminder>>())
		_reminders.insert(reminder.id(), reminder);
	emit remindersChanged(_reminders.values());
}

void TraySnoozeViewModel::onResult(quint32 requestCode, const QVariant &result)
{
	auto id = _requests.take(requestCode);
	if(!id.isNull()) {
		auto res = TermSelectionViewModel::readSingleResult(result);
		if(!res.isEmpty())
			finishSnooze(id, res);
		else
			setBlocked(false);
	}
}

void TraySnoozeViewModel::finishSnooze(QUuid id, const Expressions::Term &term)
{
	auto rem = _reminders.value(id);
	if(rem.id().isNull())
		return;

	try {
		auto when = _parser->evaluteTerm(term);
		_reminders.remove(id);
		emit _notifier->messageDelayed(id, rem.versionCode(), when);
		emit remindersChanged(_reminders.values());
		setBlocked(false);
	} catch (EventExpressionParserException &e) {
		QtMvvm::critical(tr("Snoozing failed!"), e.message());
		setBlocked(false);
	}
}

void TraySnoozeViewModel::setBlocked(bool blocked)
{
	if(blocked == _blocked)
		return;

	_blocked = blocked;
	emit blockedChanged(_blocked);
}

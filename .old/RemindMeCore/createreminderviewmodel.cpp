#include "createreminderviewmodel.h"
#include "remindmeapp.h"
#include <rep_remindermanager_replica.h>
#include <QtMvvmCore/Messages>

CreateReminderViewModel::CreateReminderViewModel(QObject *parent) :
	ViewModel(parent),
	_reminderManager(coreApp->node()->acquire<ReminderManagerReplica>()),
	_text(),
	_important(false),
	_expression()
{
	connect(_reminderManager, &ReminderManagerReplica::reminderCreated,
			this, &CreateReminderViewModel::remCreated);
	connect(_reminderManager, &ReminderManagerReplica::reminderError,
			this, &CreateReminderViewModel::remError);
}

QString CreateReminderViewModel::text() const
{
	return _text;
}

bool CreateReminderViewModel::important() const
{
	return _important;
}

QString CreateReminderViewModel::expression() const
{
	return _expression;
}

void CreateReminderViewModel::setText(const QString &text)
{
	if (_text == text)
		return;

	_text = text;
	emit textChanged(_text);
}

void CreateReminderViewModel::setImportant(bool important)
{
	if (_important == important)
		return;

	_important = important;
	emit importantChanged(_important);
}

void CreateReminderViewModel::setExpression(const QString &expression)
{
	if (_expression == expression)
		return;

	_expression = expression;
	emit expressionChanged(_expression);
}

void CreateReminderViewModel::create()
{
	auto createFn = [this](){
		_reminderManager->createReminder(_text, _important, _expression);
	};

	if(_reminderManager->isInitialized())
		createFn();
	else {
		connect(_reminderManager, &ReminderManagerReplica::initialized,
				this, createFn);
	}
}

void CreateReminderViewModel::remCreated()
{
	emit createCompleted(true);
}

void CreateReminderViewModel::remError(bool isCreate, const QString &error)
{
	if(isCreate) {
		QtMvvm::critical(tr("Failed to create reminder"), error);
		emit createCompleted(false);
	}
}

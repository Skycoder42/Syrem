#include "createremindercontrol.h"
#include "remindmeapp.h"
#include <rep_remindermanager_replica.h>
#include <coremessage.h>

CreateReminderControl::CreateReminderControl(QObject *parent) :
	Control(parent),
	_reminderManager(coreApp->node()->acquire<ReminderManagerReplica>()),
	_text(),
	_important(false),
	_expression()
{
	connect(_reminderManager, &ReminderManagerReplica::reminderCreated,
			this, &CreateReminderControl::remCreated);
	connect(_reminderManager, &ReminderManagerReplica::reminderError,
			this, &CreateReminderControl::remError);
}

QString CreateReminderControl::text() const
{
	return _text;
}

bool CreateReminderControl::important() const
{
	return _important;
}

QString CreateReminderControl::expression() const
{
	return _expression;
}

void CreateReminderControl::setText(QString text)
{
	if (_text == text)
		return;

	_text = text;
	emit textChanged(_text);
}

void CreateReminderControl::setImportant(bool important)
{
	if (_important == important)
		return;

	_important = important;
	emit importantChanged(_important);
}

void CreateReminderControl::setExpression(QString expression)
{
	if (_expression == expression)
		return;

	_expression = expression;
	emit expressionChanged(_expression);
}

void CreateReminderControl::create()
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

void CreateReminderControl::remCreated()
{
	emit createCompleted(true);
}

void CreateReminderControl::remError(bool isCreate, const QString &error)
{
	if(isCreate) {
		CoreMessage::critical(tr("Failed to create reminder"), error);
		emit createCompleted(false);
	}
}

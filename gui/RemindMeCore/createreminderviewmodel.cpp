#include "createreminderviewmodel.h"
#include <QtMvvmCore/Messages>
#include <QTimer>

CreateReminderViewModel::CreateReminderViewModel(QObject *parent) :
	ViewModel{parent},
	_store{new ReminderStore{this}}
{}

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

bool CreateReminderViewModel::create()
{
	try {
		auto terms = _parser->parseMultiExpression(_expression);
		//TODO term selection

		Reminder rem;
		rem.setDescription(_text);
		rem.setImportant(_important);
		rem.setSchedule(_parser->createMultiSchedule(terms));

		connect(_store, &QtDataSync::DataTypeStoreBase::dataChanged,
				this, [this, rem](const QString &key){
			if(key == rem.id().toString())
				emit close();
		});

		_store->save(rem);
		QTimer::singleShot(5000, this, &CreateReminderViewModel::close);
		return true;
	} catch(EventExpressionParserException &e) {
		QtMvvm::critical(tr("Failed to create reminder"), e.qWhat());
	} catch(QException &e) {
		qCritical() << "Failed to save reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to create reminder"), tr("Failed to save reminder!"));
	}

	return false;
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

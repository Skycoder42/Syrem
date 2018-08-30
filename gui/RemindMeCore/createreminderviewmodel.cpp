#include "createreminderviewmodel.h"
#include <QtMvvmCore/Messages>
#include <QTimer>
#include "termselectionviewmodel.h"

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

bool CreateReminderViewModel::isBlocked() const
{
	return _blocked;
}

void CreateReminderViewModel::create()
{
	try {
		auto terms = _parser->parseMultiExpression(_expression);
		if(_parser->needsSelection(terms)) {
			setBlocked(true);
			showForResult<TermSelectionViewModel>(TermSelectCode, TermSelectionViewModel::showParams(terms));
		} else
			finishCreate(terms);
	} catch(EventExpressionParserException &e) {
		QtMvvm::critical(tr("Failed to create reminder"), e.message());
	}
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

void CreateReminderViewModel::onResult(quint32 requestCode, const QVariant &result)
{
	if(requestCode == TermSelectCode) {
		auto res = TermSelectionViewModel::readMultiResult(result);
		if(!res.first.isEmpty())
			finishCreate(res.first, res.second);
		else
			setBlocked(false);
	}
}

void CreateReminderViewModel::finishCreate(const Expressions::MultiTerm &term, const QList<int> &choices)
{
	try {
		Reminder rem;
		rem.setDescription(_text);
		rem.setImportant(_important);
		rem.setSchedule(_parser->createMultiSchedule(term, choices));
		rem.setExpression(_expression);

		connect(_store, &QtDataSync::DataTypeStoreBase::dataChanged,
				this, [this, rem](const QString &key){
			if(key == rem.id().toString())
				emit close();
		});

		setBlocked(true);
		_store->save(rem);
		QTimer::singleShot(5000, this, &CreateReminderViewModel::close);
	} catch(EventExpressionParserException &e) {
		QtMvvm::critical(tr("Failed to create reminder"), e.message());
		setBlocked(false);
	} catch(QException &e) {
		qCritical() << "Failed to save reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to create reminder"), tr("Failed to save reminder!"));
		setBlocked(false);
	}
}

void CreateReminderViewModel::setBlocked(bool blocked)
{
	if(blocked == _blocked)
		return;

	_blocked = blocked;
	emit blockedChanged(_blocked);
}

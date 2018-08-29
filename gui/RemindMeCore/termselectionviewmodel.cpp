#include "termselectionviewmodel.h"

const QString TermSelectionViewModel::paramTerms = QStringLiteral("terms");
const QString TermSelectionViewModel::paramChoices = QStringLiteral("choices");
const QString TermSelectionViewModel::paramMulti = QStringLiteral("multi");

TermSelectionViewModel::TermSelectionViewModel(QObject *parent) :
	ViewModel{parent}
{}

QVariantHash TermSelectionViewModel::showParams(const Expressions::TermSelection &term)
{
	return {
		{paramTerms, QVariant::fromValue<Expressions::MultiTerm>({term})},
		{paramMulti, false}
	};
}

QVariantHash TermSelectionViewModel::showParams(const Expressions::MultiTerm &terms)
{
	return {
		{paramTerms, QVariant::fromValue(terms)},
		{paramMulti, terms.size() > 1}
	};
}

Expressions::Term TermSelectionViewModel::readSingleResult(const QVariant &result)
{
	if(!result.isValid())
		return {};

	auto params = result.toHash();
	return params.value(paramTerms)
			.value<Expressions::MultiTerm>()
			.first()
			.at(params.value(paramChoices).value<QList<int>>().first());
}

std::pair<Expressions::MultiTerm, QList<int>> TermSelectionViewModel::readMultiResult(const QVariant &result)
{
	if(!result.isValid())
		return {};

	auto params = result.toHash();
	return {
		params.value(paramTerms).value<Expressions::MultiTerm>(),
		params.value(paramChoices).value<QList<int>>()
	};
}

bool TermSelectionViewModel::multi() const
{
	return _multi;
}

QList<int> TermSelectionViewModel::terms() const
{
	return _indices;
}

bool TermSelectionViewModel::allHandled() const
{
	for(auto index : _indices) {
		if(!_choices.contains(index))
			return false;
	}
	return true;
}

QStringList TermSelectionViewModel::describeChoices(int termIndex) const
{
	Q_ASSERT(termIndex < _terms.size());
	QStringList choices;
	choices.reserve(_terms.at(termIndex).size());
	for(const auto &term : _terms.at(termIndex))
		choices.append(term.describe());
	return choices;
}

void TermSelectionViewModel::selectChoice(int termIndex, int choiceIndex)
{
	Q_ASSERT(termIndex < _terms.size());
	Q_ASSERT(choiceIndex < _terms.at(termIndex).size());
	_choices.insert(termIndex, choiceIndex);
	emit allHandledChanged(allHandled());
}

void TermSelectionViewModel::complete()
{
	if(!allHandled())
		return;

	QList<int> choices;
	choices.reserve(_terms.size());
	for(auto i = 0; i < _terms.size(); i++)
		choices.append(_choices.value(i, 0));

	QVariantMap result;
	result.insert(paramMulti, _multi);
	result.insert(paramTerms, QVariant::fromValue(_terms));
	result.insert(paramChoices, QVariant::fromValue(choices));
	emit resultReady(result);
}

void TermSelectionViewModel::onInit(const QVariantHash &params)
{
	_terms = params.value(paramTerms).value<Expressions::MultiTerm>();
	_multi = params.value(paramMulti).toBool();
	_indices.clear();
	_choices.clear();

	for(auto i = 0; i < _terms.size(); ++i) {
		if(_terms[i].size() > 1)
			_indices.append(i);
	}

	emit termsChanged(_indices);
	emit multiChanged(_multi);
	emit allHandledChanged(allHandled());
}

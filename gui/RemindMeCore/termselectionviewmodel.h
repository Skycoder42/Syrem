#ifndef TERMSELECTIONVIEWMODEL_H
#define TERMSELECTIONVIEWMODEL_H

#include <QtMvvmCore/ViewModel>
#include <eventexpressionparser.h>

class TermSelectionViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(bool multi READ multi NOTIFY multiChanged)
	Q_PROPERTY(QList<int> terms READ terms NOTIFY termsChanged)
	Q_PROPERTY(bool allHandled READ allHandled NOTIFY allHandledChanged)

public:
	const static QString paramTerms;
	const static QString paramChoices;
	const static QString paramMulti;

	static QVariantHash showParams(const Expressions::TermSelection &term);
	static QVariantHash showParams(const Expressions::MultiTerm &terms);

	static Expressions::Term readSingleResult(const QVariant &result);
	static std::pair<Expressions::MultiTerm, QList<int>> readMultiResult(const QVariant &result);

	Q_INVOKABLE explicit TermSelectionViewModel(QObject *parent = nullptr);

	bool multi() const;
	QList<int> terms() const;
	bool allHandled() const;

	Q_INVOKABLE QStringList describeChoices(int termIndex) const;

public slots:
	void selectChoice(int termIndex, int choiceIndex);
	void complete();

signals:
	void multiChanged(bool multi);
	void termsChanged(QList<int> terms);
	void allHandledChanged(bool allHandled);

protected:
	void onInit(const QVariantHash &params) override;

private:
	bool _multi = false;
	Expressions::MultiTerm _terms;
	QList<int> _indices;
	QHash<int, int> _choices;
};

#endif // TERMSELECTIONVIEWMODEL_H

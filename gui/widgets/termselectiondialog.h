#ifndef TERMSELECTIONDIALOG_H
#define TERMSELECTIONDIALOG_H

#include <QDialog>
#include <termselectionviewmodel.h>

namespace Ui {
class TermSelectionDialog;
}

class TermSelectionDialog : public QDialog
{
	Q_OBJECT

	Q_PROPERTY(bool showTabs READ showTabs WRITE setShowTabs)
	Q_PROPERTY(QList<int> tabs READ tabs WRITE setTabs)

public:
	Q_INVOKABLE explicit TermSelectionDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~TermSelectionDialog() override;

	bool showTabs() const;
	QList<int> tabs() const;

public slots:
	void setShowTabs(bool showTabs);
	void setTabs(const QList<int> &tabs);

	void accept() override;

private:
	TermSelectionViewModel *_viewModel;
	Ui::TermSelectionDialog *_ui;
};

#endif // TERMSELECTIONDIALOG_H

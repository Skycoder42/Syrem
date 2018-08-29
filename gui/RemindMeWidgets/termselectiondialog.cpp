#include "termselectiondialog.h"
#include "ui_termselectiondialog.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QtMvvmCore/Binding>
#include <dialogmaster.h>

TermSelectionDialog::TermSelectionDialog(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QDialog{parent},
	_viewModel{static_cast<TermSelectionViewModel*>(viewModel)},
	_ui{new Ui::TermSelectionDialog}
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this);

	QtMvvm::bind(_viewModel, "multi",
				 this, "showTabs",
				 QtMvvm::Binding::OneWayToView);
	QtMvvm::bind(_viewModel, "terms",
				 this, "tabs",
				 QtMvvm::Binding::OneWayToView);
	QtMvvm::bind(_viewModel, "allHandled",
				 _ui->buttonBox->button(QDialogButtonBox::Ok), "enabled",
				 QtMvvm::Binding::OneWayToView);
}

TermSelectionDialog::~TermSelectionDialog()
{
	delete _ui;
}

bool TermSelectionDialog::showTabs() const
{
	return !_ui->tabWidget->tabBarAutoHide();
}

QList<int> TermSelectionDialog::tabs() const
{
	return _viewModel->terms(); //dummy
}

void TermSelectionDialog::setShowTabs(bool showTabs)
{
	_ui->tabWidget->setTabBarAutoHide(!showTabs);
}

void TermSelectionDialog::setTabs(const QList<int> &tabs)
{
	_ui->tabWidget->clear();
	for(auto tab : tabs) {
		auto area = new QScrollArea{_ui->tabWidget};
		area->setBackgroundRole(QPalette::Base);
		area->setFrameShape(QFrame::NoFrame);
		auto tabWidget = new QWidget{area};
		auto layout = new QVBoxLayout{tabWidget};
		tabWidget->setLayout(layout);
		auto btnGrp = new QButtonGroup{tabWidget};
		btnGrp->setExclusive(true);
		auto index = 0;
		for(const auto &description : _viewModel->describeChoices(tab)) {
			auto btn = new QRadioButton{description, tabWidget};
			btn->setToolTip(description);
			btnGrp->addButton(btn, index++);
			layout->addWidget(btn);
		}
		connect(btnGrp, QOverload<int>::of(&QButtonGroup::buttonClicked),
				this, [this, tab](int termIndex){
			_viewModel->selectChoice(tab, termIndex);
		});

		layout->addStretch();
		area->setWidgetResizable(true);
		area->setWidget(tabWidget);
		_ui->tabWidget->addTab(area, tr("Subterm %L1").arg(tab));
	}
}

void TermSelectionDialog::accept()
{
	_viewModel->complete();
	QDialog::accept();
}

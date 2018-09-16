#include "createreminderdialog.h"
#include "ui_createreminderdialog.h"
#include <QtMvvmCore/Binding>
#include <QPushButton>
#include <dialogmaster.h>
#include <localsettings.h>

CreateReminderDialog::CreateReminderDialog(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QDialog(parent),
	_viewModel(static_cast<CreateReminderViewModel*>(viewModel)),
	_ui(new Ui::CreateReminderDialog)
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this);

	_ui->helpBrowser->setHtml(_viewModel->helpText());
	_ui->buttonBox->button(QDialogButtonBox::Help)->setCheckable(true);

	QtMvvm::bind(_viewModel, "text",
				 _ui->textLineEdit, "text",
				 QtMvvm::Binding::OneWayToViewModel);
	QtMvvm::bind(_viewModel, "expression",
				 _ui->whenLineEdit, "text",
				 QtMvvm::Binding::OneWayToViewModel);
	QtMvvm::bind(_viewModel, "important",
				 _ui->importantCheckBox, "checked",
				 QtMvvm::Binding::OneWayToViewModel);
	connect(_viewModel, &CreateReminderViewModel::blockedChanged,
			this, &CreateReminderDialog::setDisabled);
	connect(_viewModel, &CreateReminderViewModel::close,
			this, [this](){
		QDialog::accept();
	});

	if(LocalSettings::instance()->gui.createreminderdialog.size.isSet())
		resize(LocalSettings::instance()->gui.createreminderdialog.size);
	on_buttonBox_helpRequested();
}

CreateReminderDialog::~CreateReminderDialog()
{
	LocalSettings::instance()->gui.createreminderdialog.size = size();

	delete _ui;
}

void CreateReminderDialog::accept()
{
	_viewModel->create();
}

void CreateReminderDialog::on_buttonBox_helpRequested()
{
	_ui->helpBrowser->setVisible(_ui->buttonBox->button(QDialogButtonBox::Help)->isChecked());
	auto cWidth = width();
	adjustSize();
	resize(cWidth, height());
}

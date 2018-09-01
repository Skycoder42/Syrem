#include "createreminderdialog.h"
#include "ui_createreminderdialog.h"
#include <QtMvvmCore/Binding>
#include <QWhatsThis>
#include <dialogmaster.h>
#include <localsettings.h>

CreateReminderDialog::CreateReminderDialog(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QDialog(parent),
	_viewModel(static_cast<CreateReminderViewModel*>(viewModel)),
	_ui(new Ui::CreateReminderDialog)
{
	_ui->setupUi(this);
	_ui->whenLineEdit->setWhatsThis(Syrem::whenExpressionHelp());
	DialogMaster::masterDialog(this);

	_ui->whenLineEdit->addAction(_ui->actionExpression_Syntax, QLineEdit::TrailingPosition);

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

void CreateReminderDialog::on_actionExpression_Syntax_triggered()
{
	QWhatsThis::showText(QPoint(0, 0), Syrem::whenExpressionHelp(), _ui->whenLineEdit);
}

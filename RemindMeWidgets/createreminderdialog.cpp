#include "createreminderdialog.h"
#include "ui_createreminderdialog.h"
#include <dialogmaster.h>
#include <qtmvvmbinding.h>

CreateReminderDialog::CreateReminderDialog(Control *mControl, QWidget *parent) :
	QDialog(parent),
	_control(static_cast<CreateReminderControl*>(mControl)),
	_ui(new Ui::CreateReminderDialog)
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this);

	QtMvvmBinding::bind(_control, "text", _ui->textLineEdit, "text");
	QtMvvmBinding::bind(_control, "expression", _ui->whenLineEdit, "text");
	QtMvvmBinding::bind(_control, "important", _ui->importantCheckBox, "checked");

	connect(_control, &CreateReminderControl::createComplete,
			this, &CreateReminderDialog::created);
}

CreateReminderDialog::~CreateReminderDialog()
{
	delete _ui;
}

void CreateReminderDialog::accept()
{
	_ui->buttonBox->setEnabled(false);
	_control->create();
}

void CreateReminderDialog::created(bool success)
{
	if(success)
		QDialog::accept();
	else
		_ui->buttonBox->setEnabled(true);
}

#include "createreminderdialog.h"
#include "ui_createreminderdialog.h"
#include <QSettings>
#include <QWhatsThis>
#include <dialogmaster.h>
#include <qtmvvmbinding.h>

CreateReminderDialog::CreateReminderDialog(Control *mControl, QWidget *parent) :
	QDialog(parent),
	_control(static_cast<CreateReminderControl*>(mControl)),
	_ui(new Ui::CreateReminderDialog)
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this);

	_ui->whenLineEdit->addAction(_ui->actionExpression_Syntax, QLineEdit::TrailingPosition);

	QtMvvmBinding::bind(_control, "text", _ui->textLineEdit, "text");
	QtMvvmBinding::bind(_control, "expression", _ui->whenLineEdit, "text");
	QtMvvmBinding::bind(_control, "important", _ui->importantCheckBox, "checked");

	connect(_control, &CreateReminderControl::createCompleted,
			this, &CreateReminderDialog::created);

	QSettings settings;
	settings.beginGroup(QStringLiteral("gui/createreminder"));
	if(_control->parentControl()){
		auto s = settings.value(QStringLiteral("size")).toSize();
		if(s.isValid())
			resize(s);
	} else
		restoreGeometry(settings.value(QStringLiteral("geom")).toByteArray());
	settings.endGroup();
}

CreateReminderDialog::~CreateReminderDialog()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("gui/createreminder"));
	if(_control->parentControl())
		settings.setValue(QStringLiteral("size"), size());
	else
		settings.setValue(QStringLiteral("geom"), saveGeometry());
	settings.endGroup();

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

void CreateReminderDialog::on_actionExpression_Syntax_triggered()
{
	QWhatsThis::showText(QPoint(0, 0), _ui->whenLineEdit->whatsThis(), _ui->whenLineEdit);
}

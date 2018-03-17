#include "createreminderdialog.h"
#include "ui_createreminderdialog.h"
#include <QtMvvmCore/Binding>
#include <QSettings>
#include <QWhatsThis>
#include <dialogmaster.h>

CreateReminderDialog::CreateReminderDialog(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QDialog(parent),
	_viewModel(static_cast<CreateReminderViewModel*>(viewModel)),
	_ui(new Ui::CreateReminderDialog)
{
	_ui->setupUi(this);
	_ui->whenLineEdit->setWhatsThis(RemindMe::whenExpressionHelp());
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

	QSettings settings;
	settings.beginGroup(QStringLiteral("gui/createreminder"));
	if(parentWidget()){
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
	if(parentWidget())
		settings.setValue(QStringLiteral("size"), size());
	else
		settings.setValue(QStringLiteral("geom"), saveGeometry());
	settings.endGroup();

	delete _ui;
}

void CreateReminderDialog::accept()
{
	_ui->buttonBox->setEnabled(false);
	if(_viewModel->create())
		QDialog::accept();
	else
		_ui->buttonBox->setEnabled(true);
}

void CreateReminderDialog::on_actionExpression_Syntax_triggered()
{
	QWhatsThis::showText(QPoint(0, 0), RemindMe::whenExpressionHelp(), _ui->whenLineEdit);
}

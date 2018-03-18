#include "snoozedialog.h"
#include <QtMvvmCore/Binding>
#include <QSettings>
#include <dialogmaster.h>
#include <snoozetimes.h>

SnoozeDialog::SnoozeDialog(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QInputDialog(parent),
	_viewModel(static_cast<SnoozeViewModel*>(viewModel))
{
	setWindowTitle(tr("Snooze Reminder"));
	setInputMode(QInputDialog::TextInput);
	setComboBoxEditable(true);
	setEnabled(false);

	DialogMaster::masterDialog(this);

	QtMvvm::bind(_viewModel, "valid",
				 this, "enabled",
				 QtMvvm::Binding::OneWayToView);
	QtMvvm::bind(_viewModel, "description",
				 this, "labelText",
				 QtMvvm::Binding::OneWayToView);
	QtMvvm::bind(_viewModel, "snoozeTimes",
				 this, "comboBoxItems",
				 QtMvvm::Binding::OneWayToView);
}

void SnoozeDialog::accept()
{
	_viewModel->setExpression(textValue());
	if(_viewModel->snooze())
		QDialog::accept();
}

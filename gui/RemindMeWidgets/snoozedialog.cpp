#include "snoozedialog.h"
#include <QtMvvmCore/Binding>
#include <dialogmaster.h>
#include <snoozetimes.h>
#include <localsettings.h>

SnoozeDialog::SnoozeDialog(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QInputDialog{parent},
	_viewModel{static_cast<SnoozeViewModel*>(viewModel)}
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
	connect(_viewModel, &SnoozeViewModel::reminderLoaded,
			this, [this]() {
		setTextValue(comboBoxItems().isEmpty() ? QString{} : comboBoxItems().first());
	});
	connect(_viewModel, &SnoozeViewModel::close,
			this, &SnoozeDialog::close);

	if(LocalSettings::instance()->gui.snoozedialog.size.isSet())
		resize(LocalSettings::instance()->gui.snoozedialog.size);
}

SnoozeDialog::~SnoozeDialog()
{
	LocalSettings::instance()->gui.snoozedialog.size = size();
}

void SnoozeDialog::accept()
{
	_viewModel->setExpression(textValue());
	_viewModel->snooze();
}

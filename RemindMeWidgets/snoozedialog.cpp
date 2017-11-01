#include "snoozedialog.h"
#include <QSettings>
#include <dialogmaster.h>
#include <snoozetimes.h>
#include <qtmvvmbinding.h>

SnoozeDialog::SnoozeDialog(Control *control, QWidget *parent) :
	QInputDialog(parent),
	_control(static_cast<SnoozeControl*>(control))
{
	setWindowTitle(tr("Snooze Reminder"));
	setInputMode(QInputDialog::TextInput);
	setComboBoxEditable(true);
	setEnabled(false);

	DialogMaster::masterDialog(this);

	QtMvvmBinding::bind(_control, "loaded", this, "enabled", QtMvvmBinding::OneWayFromControl);
	QtMvvmBinding::bind(_control, "description", this, "labelText", QtMvvmBinding::OneWayFromControl);
	QtMvvmBinding::bind(_control, "snoozeTimes", this, "comboBoxItems", QtMvvmBinding::OneWayFromControl);
}

void SnoozeDialog::accept()
{
	_control->setExpression(textValue());
	_control->snooze();
}

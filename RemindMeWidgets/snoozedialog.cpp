#include "snoozedialog.h"
#include <QSettings>
#include <dialogmaster.h>
#include <snoozetimes.h>
#include <qtmvvmbinding.h>

SnoozeDialog::SnoozeDialog(Control *control, QWidget *parent) :
	QInputDialog(parent),
	_control(static_cast<SnoozeControl*>(control)),
	_title(QStringLiteral("dummy"))
{
	setWindowTitle(tr("Snooze Reminder"));
	setInputMode(QInputDialog::TextInput);
	setComboBoxEditable(true);
	setEnabled(false);

	DialogMaster::masterDialog(this);

	QtMvvmBinding::bind(_control, "snoozeTimes", this, "comboBoxItems", QtMvvmBinding::OneWayFromControl);
	QtMvvmBinding::bind(_control, "description", this, "title", QtMvvmBinding::OneWayFromControl);
}

void SnoozeDialog::accept()
{
	_control->setExpression(textValue());
	_control->snooze();
}

QString SnoozeDialog::title() const
{
	return _title;
}

void SnoozeDialog::setTitle(QString title)
{
	if (_title == title)
		return;

	_title = title;
	emit titleChanged(_title);

	if(title.isNull()) {
		setLabelText(tr("<i>Loading Reminder, please wait…</i>"));
		setEnabled(false);
	} else {
		setLabelText(tr("Choose a snooze time for the reminder:<br/>"
						"<i>%1</i>")
					 .arg(title));
		setEnabled(true);
	}
}

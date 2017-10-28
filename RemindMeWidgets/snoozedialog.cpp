#include "snoozedialog.h"
#include <QSettings>
#include <dialogmaster.h>
#include <snoozetimes.h>

SnoozeDialog::SnoozeDialog(Control *control, QWidget *parent) :
	QInputDialog(parent),
	_control(static_cast<SnoozeControl*>(control))
{
	setWindowTitle(tr("Snooze Reminder"));
	setInputMode(QInputDialog::TextInput);
	setComboBoxEditable(true);
	setComboBoxItems(_control->snoozeTimes());

	setEnabled(false);
	auto setFn = [this](const QString &text) {
		setLabelText(tr("Choose a snooze time for the reminder:<br/>"
						"<i>%1</i>")
					 .arg(text));
		setEnabled(true);
	};
	if(_control->description().isEmpty())
		setLabelText(tr("<i>Loading Reminder, please wait…</i>"));
	else
		setFn(_control->description());

	DialogMaster::masterDialog(this);

	connect(_control, &SnoozeControl::snoozeTimesChanged,
			this, &SnoozeDialog::setComboBoxItems);
	connect(_control, &SnoozeControl::descriptionChanged,
			this, setFn);
}

void SnoozeDialog::accept()
{
//	auto expression = _parser->parse(textValue());
//	if(!expression) {
//		DialogMaster::critical(this,
//							   tr("The entered text is not a valid expression. Error message:\n%1").arg(_parser->lastError()),
//							   tr("Invalid Snooze"));
//		return;
//	}

//	auto schedule = expression->createSchedule(QDateTime::currentDateTime(), _settings->value(QStringLiteral("defaultTime"), QTime(9,0)).toTime(), this);
//	if(!schedule) {
//		DialogMaster::critical(this,
//							   tr("Given expression is valid, but evaluates to a timepoint in the past!"),
//							   tr("Invalid Snooze"));
//		return;
//	}

//	if(schedule->isRepeating()) {
//		if(DialogMaster::warning(this,
//								 tr("Given expression evaluates to more the 1 timepoint. Only the closest is used, all other will be discarded"),
//								 tr("Invalid Snooze"),
//								 QString(),
//								 QMessageBox::Ok | QMessageBox::Cancel)
//		   == QMessageBox::Cancel)
//			return;
//	}

//	_nextTime = schedule->nextSchedule();
//	if(!_nextTime.isValid()) {
//		_nextTime = {};
//		DialogMaster::critical(this,
//							   tr("Given expression is valid, but evaluates to a timepoint in the past!"),
//							   tr("Invalid Snooze"));
//		return;
//	}

	QDialog::accept();
}

#include "kdesnoozedialog.h"
#include <dialogmaster.h>

KdeSnoozeDialog::KdeSnoozeDialog(const QString &text, QWidget *parent) :
	QInputDialog(parent),
	_parser(new DateParser(this)),
	_nextTime()
{
	setWindowTitle(tr("Snooze Reminder"));
	setInputMode(QInputDialog::TextInput);
	setComboBoxEditable(true);
	setComboBoxItems({
						 tr("in 20 minutes"),
						 tr("in 1 hour"),
						 tr("in 3 hours"),
						 tr("tomorrow"),
						 tr("in 1 week on Monday")
					 });
	setLabelText(tr("Choose a snooze time for the reminder:<br/>"
					"<i>%1</i>")
				 .arg(text));

	DialogMaster::masterDialog(this);
}

QDateTime KdeSnoozeDialog::snoozeTime() const
{
	return _nextTime;
}

void KdeSnoozeDialog::accept()
{
	auto expression = _parser->parse(textValue());
	if(!expression) {
		DialogMaster::critical(this,
							   tr("The entered text is not a valid expression. Error message:\n%1").arg(_parser->lastError()),
							   tr("Invalid Snooze"));
		return;
	}

	auto schedule = expression->createSchedule(QDateTime::currentDateTime(), this);
	if(!schedule) {
		DialogMaster::critical(this,
							   tr("Given expression is valid, but evaluates to a timepoint in the past!"),
							   tr("Invalid Snooze"));
		return;
	}

	if(schedule->isRepeating()) {
		if(DialogMaster::warning(this,
								 tr("Given expression evaluates to more the 1 timepoint. Only the closest is used, all other will be discarded"),
								 tr("Invalid Snooze"),
								 QString(),
								 QMessageBox::Ok | QMessageBox::Cancel)
		   == QMessageBox::Cancel)
			return;
	}

	_nextTime = schedule->nextSchedule();
	if(!_nextTime.isValid()) {
		_nextTime = {};
		DialogMaster::critical(this,
							   tr("Given expression is valid, but evaluates to a timepoint in the past!"),
							   tr("Invalid Snooze"));
		return;
	}

	QDialog::accept();
}

#include "snoozedialog.h"
#include "ui_snoozedialog.h"
#include <dialogmaster.h>

SnoozeDialog::SnoozeDialog(QWidget *parent) :
	QDialog(parent),
	_ui(new Ui::SnoozeDialog),
	_parser(new DateParser(this)),
	_nextTime()
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this);
}

SnoozeDialog::~SnoozeDialog()
{
	delete _ui;
}

QDateTime SnoozeDialog::snoozeTime() const
{
	return _nextTime;
}

void SnoozeDialog::accept()
{
	auto expression = _parser->parse(_ui->comboBox->currentText());
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

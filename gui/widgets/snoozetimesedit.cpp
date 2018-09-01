#include "snoozetimesedit.h"
#include "ui_snoozetimesedit.h"

SnoozeTimesEdit::SnoozeTimesEdit(QWidget *parent) :
	QWidget{parent},
	_ui{new Ui::SnoozeTimesEdit{}}
{
	_ui->setupUi(this);

	_ui->addButton->setDefaultAction(_ui->action_Add_Time);
	_ui->removeButton->setDefaultAction(_ui->action_Remove_Time);
	_ui->upButton->setDefaultAction(_ui->actionMove_Up);
	_ui->downButton->setDefaultAction(_ui->actionMove_Down);

	auto sep = new QAction{this};
	sep->setSeparator(true);
	_ui->listWidget->addActions({
									_ui->action_Add_Time,
									sep,
									_ui->action_Remove_Time,
									_ui->actionMove_Up,
									_ui->actionMove_Down
								});
}

SnoozeTimesEdit::~SnoozeTimesEdit()
{
	delete _ui;
}

SnoozeTimes SnoozeTimesEdit::times() const
{
	QStringList times;
	times.reserve(_ui->listWidget->count());
	for(auto i = 0; i < _ui->listWidget->count(); i++)
		times.append(_ui->listWidget->item(i)->text());
	return times;
}

void SnoozeTimesEdit::setTimes(const SnoozeTimes &times)
{
	if(times.isEmpty())
		return;
	_ui->listWidget->clear();
	for(const auto& time : times) {
		auto item = new QListWidgetItem(time, _ui->listWidget);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
}

void SnoozeTimesEdit::on_action_Add_Time_triggered()
{
	auto item = new QListWidgetItem(_ui->listWidget);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	_ui->listWidget->setCurrentItem(item);
	_ui->listWidget->editItem(item);
}

void SnoozeTimesEdit::on_action_Remove_Time_triggered()
{
	auto current = _ui->listWidget->currentItem();
	if(current)
		delete current;
}

void SnoozeTimesEdit::on_actionMove_Up_triggered()
{
	auto current = _ui->listWidget->currentItem();
	if(current) {
		auto index = _ui->listWidget->row(current);
		if(index != 0) {
			_ui->listWidget->takeItem(index);
			_ui->listWidget->insertItem(index - 1, current);
			_ui->listWidget->setCurrentItem(current);
		}
	}
}

void SnoozeTimesEdit::on_actionMove_Down_triggered()
{
	auto current = _ui->listWidget->currentItem();
	if(current) {
		auto index = _ui->listWidget->row(current);
		if(index < _ui->listWidget->count() - 1) {
			_ui->listWidget->takeItem(index);
			_ui->listWidget->insertItem(index + 1, current);
			_ui->listWidget->setCurrentItem(current);
		}
	}
}

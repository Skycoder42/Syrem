#include "widgetssnoozedialog.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <dialogmaster.h>

WidgetsSnoozeDialog::WidgetsSnoozeDialog(bool showDefaults, QWidget *parent) :
	QDialog(parent),
	_showDefaults(showDefaults),
	_toolBox(nullptr),
	_reminders(),
	_parser(new DateParser(this))
{
	setupUi();
}

void WidgetsSnoozeDialog::addReminders(const QList<Reminder> &reminders)
{
	foreach(auto rem, reminders)
		addReminder(rem);
	resizeUi();
}

void WidgetsSnoozeDialog::reject()
{
	emit aborted(_reminders.values());
	QDialog::reject();
}

void WidgetsSnoozeDialog::performComplete()
{
	auto remWidget = _toolBox->currentWidget();
	if(remWidget) {
		_toolBox->removeItem(_toolBox->currentIndex());
		auto reminder = _reminders.take(remWidget);

		emit reacted(reminder.id(), CompleteAction);
		remWidget->deleteLater();
		resizeUi();
	}
}

void WidgetsSnoozeDialog::performDefaultSnooze()
{
	auto remWidget = _toolBox->currentWidget();
	if(remWidget) {
		_toolBox->removeItem(_toolBox->currentIndex());
		auto reminder = _reminders.take(remWidget);

		emit reacted(reminder.id(), DefaultSnoozeAction);
		remWidget->deleteLater();
		resizeUi();
	}
}

void WidgetsSnoozeDialog::performSnooze()
{
	auto remWidget = _toolBox->currentWidget();
	if(remWidget) {
		auto cBox = remWidget->findChild<QComboBox*>(QString(), Qt::FindDirectChildrenOnly);
		if(!cBox)
			return;

		auto when = tryParse(cBox->currentText());
		if(!when.isValid())
			return;

		_toolBox->removeItem(_toolBox->currentIndex());
		auto reminder = _reminders.take(remWidget);

		emit reacted(reminder.id(), SnoozeAction, when);
		remWidget->deleteLater();
		resizeUi();
	}
}

void WidgetsSnoozeDialog::setupUi()
{
	setWindowTitle(tr("Triggered Reminders"));

	auto layout = new QVBoxLayout(this);
	setLayout(layout);

	auto label = new QLabel(tr("Please select an action for each of your triggered reminders:"), this);

	_toolBox = new QToolBox(this);
	_toolBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	layout->addWidget(label);
	layout->addWidget(_toolBox);

	adjustSize();
	DialogMaster::masterDialog(this, true);
}

void WidgetsSnoozeDialog::resizeUi()
{
	if(_toolBox->count() == 0)
		close();
	else {
		auto cWidth = width();
		adjustSize();
		resize(cWidth, height());
		setMaximumHeight(sizeHint().height());
	}
}

void WidgetsSnoozeDialog::addReminder(const Reminder reminder)
{
	auto remWidet = new QWidget(_toolBox);
	auto remLayout = new QHBoxLayout(remWidet);

	QPushButton *cButton = nullptr;
	QPushButton *dButton = nullptr;
	if(_showDefaults) {
		cButton = new QPushButton(remWidet);
		cButton->setText(tr("&Complete"));
		cButton->setAutoDefault(false);
		cButton->setDefault(false);
		connect(cButton, &QPushButton::clicked,
				this, &WidgetsSnoozeDialog::performComplete);

		connect(_toolBox, &QToolBox::currentChanged, cButton, [this, remWidet, cButton](int index){
			auto w = _toolBox->widget(index);
			if(w == remWidet)
				cButton->setDefault(true);
			else
				cButton->setDefault(false);
		});

		dButton = new QPushButton(remWidet);
		dButton->setText(tr("&Default Snooze"));
		dButton->setAutoDefault(false);
		dButton->setDefault(false);
		connect(dButton, &QPushButton::clicked,
				this, &WidgetsSnoozeDialog::performDefaultSnooze);
	}

	auto cBox = new QComboBox(remWidet);
	cBox->setEditable(true);
	cBox->addItems({
					   tr("in 20 minutes"),
					   tr("in 1 hour"),
					   tr("in 3 hours"),
					   tr("tomorrow"),
					   tr("in 1 week on Monday")
				   });

	auto sButton = new QPushButton(remWidet);
	sButton->setText(tr("&Snooze"));
	sButton->setAutoDefault(false);
	sButton->setDefault(false);
	connect(sButton, &QPushButton::clicked,
			this, &WidgetsSnoozeDialog::performSnooze);

	if(_showDefaults) {
		remLayout->addWidget(cButton);
		remLayout->addWidget(dButton);
		remLayout->addWidget(cBox);
		remLayout->addWidget(sButton);
		remLayout->setStretch(0, 0);
		remLayout->setStretch(1, 0);
		remLayout->setStretch(2, 1);
		remLayout->setStretch(3, 0);
	} else {
		remLayout->addWidget(cBox);
		remLayout->addWidget(sButton);
		remLayout->setStretch(0, 1);
		remLayout->setStretch(1, 0);
	}

	_toolBox->addItem(remWidet,
					  reminder.isImportant() ?
						  QIcon::fromTheme(QStringLiteral("emblem-important-symbolic"), QIcon(QStringLiteral(":/icons/important.ico"))) :
						  QIcon(QStringLiteral(":/icons/empty.ico")),
					  reminder.text());

	_reminders.insert(remWidet, reminder);
}

QDateTime WidgetsSnoozeDialog::tryParse(const QString &text)
{
	auto expression = _parser->parse(text);
	if(!expression) {
		DialogMaster::critical(this,
							   tr("The entered text is not a valid expression. Error message:\n%1").arg(_parser->lastError()),
							   tr("Invalid Snooze"));
		return {};
	}

	auto schedule = expression->createSchedule(QDateTime::currentDateTime(), this);
	if(!schedule) {
		DialogMaster::critical(this,
							   tr("Given expression is valid, but evaluates to a timepoint in the past!"),
							   tr("Invalid Snooze"));
		return {};
	}

	if(schedule->isRepeating()) {
		if(DialogMaster::warning(this,
								 tr("Given expression evaluates to more the 1 timepoint. Only the closest is used, all other will be discarded"),
								 tr("Invalid Snooze"),
								 QString(),
								 QMessageBox::Ok | QMessageBox::Cancel)
		   == QMessageBox::Cancel)
			return {};
	}

	auto when = schedule->nextSchedule();
	if(!when.isValid()) {
		DialogMaster::critical(this,
							   tr("Given expression is valid, but evaluates to a timepoint in the past!"),
							   tr("Invalid Snooze"));
		return {};
	}

	return when;
}

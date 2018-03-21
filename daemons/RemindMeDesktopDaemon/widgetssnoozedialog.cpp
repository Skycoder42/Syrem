#include "widgetssnoozedialog.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <dialogmaster.h>
#include <snoozetimes.h>

WidgetsSnoozeDialog::WidgetsSnoozeDialog(SyncedSettings *settings, DateParser *parser, QWidget *parent) :
	QDialog(parent),
	_settings(settings),
	_parser(parser),
	_toolBox(nullptr),
	_reminders()
{
	setupUi();
}

void WidgetsSnoozeDialog::addReminders(const QList<Reminder> &reminders)
{
	for(auto rem : reminders)
		addReminder(rem);
	resizeUi();
}

void WidgetsSnoozeDialog::reject()
{
	emit completed(_reminders.values());
	QDialog::accept();
}

void WidgetsSnoozeDialog::performComplete()
{
	auto remWidget = _toolBox->currentWidget();
	if(remWidget) {
		_toolBox->removeItem(_toolBox->currentIndex());
		auto reminder = _reminders.take(remWidget);

		emit reacted(reminder, true);
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

		try {
			auto when = _parser->snoozeParse(cBox->currentText());

			_toolBox->removeItem(_toolBox->currentIndex());
			auto reminder = _reminders.take(remWidget);

			emit reacted(reminder, false, when);
			remWidget->deleteLater();
			resizeUi();
		} catch (DateParserException &e) {
			DialogMaster::critical(this, e.qWhat(), tr("Snoozing failed!"));
		}
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

	// snooze combobox
	auto cBox = new QComboBox(remWidet);
	cBox->setEditable(true);
	cBox->addItems(_settings->scheduler.snoozetimes);

	//snooze button
	auto sButton = new QPushButton(remWidet);
	sButton->setText(tr("&Snooze"));
	sButton->setAutoDefault(false);
	sButton->setDefault(false);
	connect(sButton, &QPushButton::clicked,
			this, &WidgetsSnoozeDialog::performSnooze);

	// complete button
	auto cButton = new QPushButton(remWidet);
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

	remLayout->addWidget(cBox);
	remLayout->addWidget(sButton);
	remLayout->addWidget(cButton);
	remLayout->setStretch(0, 1);
	remLayout->setStretch(1, 0);
	remLayout->setStretch(2, 0);

	_toolBox->addItem(remWidet,
					  reminder.isImportant() ?
						  QIcon::fromTheme(QStringLiteral("emblem-important-symbolic"), QIcon(QStringLiteral(":/icons/important.ico"))) :
						  QIcon(QStringLiteral(":/icons/empty.ico")),
					  reminder.description());

	_reminders.insert(remWidet, reminder);
}

#include "traysnoozedialog.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtMvvmCore/Binding>
#include <dialogmaster.h>
#include <snoozetimes.h>

TraySnoozeDialog::TraySnoozeDialog(QtMvvm::ViewModel *viewModel, QWidget *parent):
	QDialog{parent},
	_viewModel{static_cast<TraySnoozeViewModel*>(viewModel)}
{
	setupUi();
	connect(_viewModel, &TraySnoozeViewModel::remindersChanged,
			this, &TraySnoozeDialog::reloadReminders);
	connect(_viewModel, &TraySnoozeViewModel::blockedChanged,
			this, &TraySnoozeDialog::setDisabled);
}

void TraySnoozeDialog::reject()
{
	QDialog::accept();
}

void TraySnoozeDialog::performComplete()
{
	auto remWidget = _toolBox->currentWidget();
	if(remWidget)
		_viewModel->performComplete(_reminders.value(remWidget));
}

void TraySnoozeDialog::performSnooze()
{
	auto remWidget = _toolBox->currentWidget();
	if(remWidget) {
		auto cBox = remWidget->findChild<QComboBox*>(QString(), Qt::FindDirectChildrenOnly);
		if(!cBox)
			return;
		_viewModel->performSnooze(_reminders.value(remWidget), cBox->currentText());
	}
}

void TraySnoozeDialog::performUrlOpen()
{
	auto remWidget = _toolBox->currentWidget();
	if(remWidget)
		_viewModel->openUrls(_reminders.value(remWidget));
}

void TraySnoozeDialog::reloadReminders(const QList<Reminder> &reminders)
{
	while(_toolBox->count() > 0) {
		auto w = _toolBox->widget(0);
		_toolBox->removeItem(0);
		w->deleteLater();
	}

	_reminders.clear();
	for(const auto &rem : reminders)
		addReminder(rem);
	resizeUi();
}

void TraySnoozeDialog::setupUi()
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

void TraySnoozeDialog::resizeUi()
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

void TraySnoozeDialog::addReminder(const Reminder &reminder)
{
	auto remWidget = new QWidget{_toolBox};
	auto remLayout = new QHBoxLayout{remWidget};

	// snooze combobox
	auto cBox = new QComboBox{remWidget};
	cBox->setEditable(true);
	cBox->addItems(_viewModel->settings()->scheduler.snoozetimes);
	remLayout->addWidget(cBox);

	//snooze button
	auto sButton = new QPushButton{tr("&Snooze"), remWidget};
	sButton->setAutoDefault(false);
	sButton->setDefault(false);
	connect(sButton, &QPushButton::clicked,
			this, &TraySnoozeDialog::performSnooze);
	remLayout->addWidget(sButton);

	// complete button
	auto cButton = new QPushButton{tr("&Complete"), remWidget};
	cButton->setAutoDefault(false);
	cButton->setDefault(false);
	connect(cButton, &QPushButton::clicked,
			this, &TraySnoozeDialog::performComplete);
	connect(_toolBox, &QToolBox::currentChanged, cButton, [this, remWidget, cButton](int index){
		auto w = _toolBox->widget(index);
		if(w == remWidget)
			cButton->setDefault(true);
		else
			cButton->setDefault(false);
	});
	remLayout->addWidget(cButton);

	// open urls button
	if(reminder.hasUrls()) {
		auto uButton = new QPushButton{tr("Open &URLs"), remWidget};
		uButton->setAutoDefault(false);
		uButton->setDefault(false);
		connect(uButton, &QPushButton::clicked,
				this, &TraySnoozeDialog::performUrlOpen);
		remLayout->addWidget(uButton);
	}

	remLayout->setStretch(0, 1);
	remLayout->setStretch(1, 0);
	remLayout->setStretch(2, 0);
	if(reminder.hasUrls())
		remLayout->setStretch(3, 0);

	QIcon icon;
	if(reminder.isImportant())
		icon = QIcon::fromTheme(QStringLiteral("emblem-important-symbolic"), QIcon(QStringLiteral(":/icons/important.ico")));
	_toolBox->addItem(remWidget, icon, reminder.description());

	_reminders.insert(remWidget, reminder.id());
}

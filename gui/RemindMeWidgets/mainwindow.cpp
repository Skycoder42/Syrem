#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtMvvmCore/Binding>

MainWindow::MainWindow(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QMainWindow(parent),
	_viewModel(static_cast<MainViewModel*>(viewModel)),
	_ui(new Ui::MainWindow)
{
	_ui->setupUi(this);
	setCentralWidget(_ui->treeView);

	connect(_ui->action_Close, &QAction::triggered,
			this, &MainWindow::close);
	connect(_ui->action_Settings, &QAction::triggered,
			_viewModel, &MainViewModel::showSettings);
	connect(_ui->actionS_ynchronization, &QAction::triggered,
			_viewModel, &MainViewModel::showSync);
	connect(_ui->action_About, &QAction::triggered,
			_viewModel, &MainViewModel::showAbout);
	connect(_ui->action_Add_Reminder, &QAction::triggered,
			_viewModel, &MainViewModel::addReminder);

	auto sep = new QAction(this);
	sep->setSeparator(true);
	_ui->treeView->addActions({
								  _ui->action_Add_Reminder,
								  _ui->action_Delete_Reminder,
								  sep,
								  _ui->action_Complete_Reminder,
								  _ui->action_Snooze_Reminder
							  });

	_ui->treeView->setModel(_viewModel->reminderModel());

	connect(_ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &MainWindow::updateCurrent);
}

MainWindow::~MainWindow()
{
	delete _ui;
}

void MainWindow::on_action_Complete_Reminder_triggered()
{
	_viewModel->completeReminder(idFromIndex(_ui->treeView->currentIndex()));
}

void MainWindow::on_action_Delete_Reminder_triggered()
{
	_viewModel->deleteReminder(idFromIndex(_ui->treeView->currentIndex()));
}

void MainWindow::on_action_Snooze_Reminder_triggered()
{
	_viewModel->snoozeReminder(idFromIndex(_ui->treeView->currentIndex()));
}

void MainWindow::on_treeView_activated(const QModelIndex &index)
{
	_viewModel->snoozeReminder(idFromIndex(index));
}

void MainWindow::updateCurrent(const QModelIndex &index)
{
	if(index.isValid()) {
		auto reminder = _viewModel->reminderModel()->object<Reminder>(index);
		auto state = reminder.triggerState();
		auto canSnooze = state == Reminder::Triggered;
		auto isReady = (canSnooze || state == Reminder::Snoozed);
		_ui->action_Delete_Reminder->setVisible(!isReady);
		_ui->action_Complete_Reminder->setVisible(isReady);
		_ui->action_Snooze_Reminder->setVisible(canSnooze);
	} else {
		_ui->action_Delete_Reminder->setVisible(true);
		_ui->action_Complete_Reminder->setVisible(false);
		_ui->action_Snooze_Reminder->setVisible(false);
	}
}

QUuid MainWindow::idFromIndex(const QModelIndex &index)
{
	if(!index.isValid())
		return {};
	return _viewModel->reminderModel()->object<Reminder>(index).id();
}

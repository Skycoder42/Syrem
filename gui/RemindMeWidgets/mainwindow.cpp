#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtMvvmCore/Binding>

MainWindow::MainWindow(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QMainWindow(parent),
	_viewModel(static_cast<MainViewModel*>(viewModel)),
	_ui(new Ui::MainWindow),
	_proxyModel(new ReminderProxyModel(this)),
	_sortModel(new QSortFilterProxyModel(this))
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

	_proxyModel->setSourceModel(_viewModel->reminderModel());
	_sortModel->setSourceModel(_proxyModel);
	_ui->treeView->setModel(_sortModel);

	_ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	_ui->treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

	connect(_ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &MainWindow::updateCurrent);
}

MainWindow::~MainWindow()
{
	delete _ui;
}

void MainWindow::on_action_Complete_Reminder_triggered()
{
	_viewModel->completeReminder(reminderAt(_ui->treeView->currentIndex()).id());
}

void MainWindow::on_action_Delete_Reminder_triggered()
{
	_viewModel->deleteReminder(reminderAt(_ui->treeView->currentIndex()).id());
}

void MainWindow::on_action_Snooze_Reminder_triggered()
{
	_viewModel->snoozeReminder(reminderAt(_ui->treeView->currentIndex()).id());
}

void MainWindow::on_treeView_activated(const QModelIndex &index)
{
	_viewModel->snoozeReminder(reminderAt(index).id());
}

void MainWindow::updateCurrent(const QModelIndex &index)
{
	if(index.isValid()) {
		auto reminder = reminderAt(index);
		auto state = reminder.triggerState();
		auto canSnooze = state == Reminder::Triggered;
		auto isReady = (canSnooze || state == Reminder::Snoozed);
		_ui->action_Complete_Reminder->setVisible(isReady);
		_ui->action_Snooze_Reminder->setVisible(canSnooze);
	} else {
		_ui->action_Complete_Reminder->setVisible(false);
		_ui->action_Snooze_Reminder->setVisible(false);
	}
}

Reminder MainWindow::reminderAt(const QModelIndex &sIndex)
{
	if(!sIndex.isValid())
		return {};

	auto pIndex = _sortModel->mapToSource(sIndex);
	if(!pIndex.isValid())
		return {};

	auto index = _proxyModel->mapToSource(pIndex);
	if(!index.isValid())
		return {};

	return _viewModel->reminderModel()->object<Reminder>(index);
}



ReminderProxyModel::ReminderProxyModel(QObject *parent) :
	QObjectProxyModel({tr("Reminder"), tr("Due on")}, parent),
	_settings(new QSettings(this))
{}

QVariant ReminderProxyModel::data(const QModelIndex &index, int role) const
{
	auto data = QObjectProxyModel::data(index, role);
	if(!data.isValid())
		return {};

	auto format = (QLocale::FormatType)_settings->value(QStringLiteral("gui/dateformat"), QLocale::ShortFormat).toInt();
	switch (index.column()) {
	case 0:
		if(role == Qt::DecorationRole) {
			if(data.toBool())
				return QIcon::fromTheme(QStringLiteral("emblem-important-symbolic"), QIcon(QStringLiteral(":/icons/important.ico")));
			else
				return QIcon(QStringLiteral(":/icons/empty.ico"));
		} else if(role == Qt::ToolTipRole) {
			auto important = QObjectProxyModel::data(index, Qt::DecorationRole).toBool();
			if(important)
				return data.toString().append(tr("<br/><i>This is an important reminder</i>"));
		}
		break;
	case 1:
		if(role == Qt::DecorationRole) {
			switch(data.toInt()) {
			case Reminder::Normal:
				return QIcon(QStringLiteral(":/icons/empty.ico"));
			case Reminder::NormalRepeating:
				return QIcon::fromTheme(QStringLiteral("media-playlist-repeat"), QIcon(QStringLiteral(":/icons/loop.ico")));
			case Reminder::Snoozed:
				return QIcon::fromTheme(QStringLiteral("alarm-symbolic"), QIcon(QStringLiteral(":/icons/snooze.ico")));
			case Reminder::Triggered:
				return QIcon::fromTheme(QStringLiteral("view-calendar-upcoming-events"), QIcon(QStringLiteral(":/icons/trigger.ico")));
			default:
				break;
			}
		} else if(role == Qt::ToolTipRole) {
			auto dateTime = QObjectProxyModel::data(index, Qt::DisplayRole);
			auto baseStr = QLocale().toString(dateTime.toDateTime(), QLocale::LongFormat);
			switch(data.toInt()) {
			case Reminder::Normal:
				return baseStr;
			case Reminder::NormalRepeating:
				return baseStr.append(tr("\nReminder will repeatedly trigger, not only once"));
			case Reminder::Snoozed:
				return baseStr.append(tr("\nReminder has been snoozed until the displayed time"));
			case Reminder::Triggered:
				return baseStr.append(tr("\nReminder has been triggered and needs a reaction!"));
			default:
				break;
			}
		} else if(role == Qt::DisplayRole)
			return QLocale().toString(data.toDateTime(), format);
		break;
	default:
		break;
	}

	return data;
}

void ReminderProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
	QObjectProxyModel::setSourceModel(sourceModel);
	addMapping(0, Qt::DecorationRole, "important");
	addMapping(0, Qt::DisplayRole, "description");
	addMapping(0, Qt::ToolTipRole, "description");
	addMapping(1, Qt::DisplayRole, "current");
	addMapping(1, Qt::DecorationRole, "triggerState");
	addMapping(1, Qt::ToolTipRole, "triggerState");
}

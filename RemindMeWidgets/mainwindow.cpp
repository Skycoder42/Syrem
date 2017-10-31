#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <dialogmaster.h>

MainWindow::MainWindow(Control *mControl, QWidget *parent) :
	QMainWindow(parent),
	_control(static_cast<MainControl*>(mControl)),
	_ui(new Ui::MainWindow),
	_proxyModel(new ReminderProxyModel(this)),
	_sortModel(new QSortFilterProxyModel(this))
{
	_ui->setupUi(this);
	setCentralWidget(_ui->treeView);
	_ui->centralWidget->deleteLater();

	connect(_ui->action_Complete_Reminder, &QAction::triggered,
			this, &MainWindow::on_action_Delete_Reminder_triggered);//same as delete

	connect(_ui->action_Close, &QAction::triggered,
			this, &MainWindow::close);
	connect(_ui->action_Quit, &QAction::triggered,
			qApp, &QApplication::quit);
	connect(_ui->action_Settings, &QAction::triggered,
			_control, &MainControl::showSettings);
	connect(_ui->action_Add_Reminder, &QAction::triggered,
			_control, &MainControl::addReminder);

	auto sep = new QAction(this);
	sep->setSeparator(true);
	_ui->treeView->addActions({
								  _ui->action_Add_Reminder,
								  _ui->action_Delete_Reminder,
								  sep,
								  _ui->action_Complete_Reminder,
								  _ui->action_Snooze_Reminder
							  });

	auto remModel = _control->reminderModel();
	auto initFn = [this](){
		_proxyModel->setSourceModel(_control->reminderModel());

		_sortModel->setSourceModel(_proxyModel);
		_ui->treeView->setModel(_sortModel);

		_ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
		_ui->treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

		connect(_ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
				this, &MainWindow::updateCurrent);
	};

	if(remModel->isInitialized())
		initFn();
	else
		connect(remModel, &QAbstractItemModelReplica::initialized, this, initFn);
}

MainWindow::~MainWindow()
{
	delete _ui;
}

void MainWindow::on_action_Delete_Reminder_triggered()
{
	_control->removeReminder(idFromIndex(_ui->treeView->currentIndex()));
}

void MainWindow::on_action_Snooze_Reminder_triggered()
{
	on_treeView_activated(_ui->treeView->currentIndex());
}

void MainWindow::on_action_About_triggered()
{
	DialogMaster::about(this,
						tr("A simple reminder application for desktop and mobile, with synchronized reminder."),
						QStringLiteral("https://github.com/Skycoder42/RemindMe"),
						tr("BSD 3 Clause"),
						QStringLiteral("https://github.com/Skycoder42/RemindMe/blob/master/LICENSE"));
}

void MainWindow::on_treeView_activated(const QModelIndex &index)
{
	if(!index.isValid())
		return;
	auto mIndex = indexFromIndex(index);
	auto mRole = _control->reminderModel()->roleNames().key("triggerState");
	auto mData = _control->reminderModel()->data(mIndex, mRole).toInt();
	if(mData == 3)
		_control->snoozeReminder(idFromIndex(index));
}

void MainWindow::updateCurrent(const QModelIndex &index)
{
	auto mIndex = indexFromIndex(index);
	if(mIndex.isValid()) {
		auto mRole = _control->reminderModel()->roleNames().key("triggerState");
		auto mData = _control->reminderModel()->data(mIndex, mRole).toInt();
		auto isReady = (mData == 3);
		_ui->action_Delete_Reminder->setVisible(!isReady);
		_ui->action_Complete_Reminder->setVisible(isReady);
		_ui->action_Snooze_Reminder->setVisible(isReady);
	} else {
		_ui->action_Delete_Reminder->setVisible(true);
		_ui->action_Complete_Reminder->setVisible(false);
		_ui->action_Snooze_Reminder->setVisible(false);
	}
}

QModelIndex MainWindow::indexFromIndex(const QModelIndex &sIndex)
{
	if(!sIndex.isValid())
		return {};

	auto pIndex = _sortModel->mapToSource(sIndex);
	if(!pIndex.isValid())
		return {};

	auto index = _proxyModel->mapToSource(pIndex);
	if(!index.isValid())
		return {};
	return index;
}

QUuid MainWindow::idFromIndex(const QModelIndex &sIndex)
{
	auto index = indexFromIndex(sIndex);
	if(!index.isValid())
		return {};
	return _control->reminderModel()->data(index).toUuid();
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
		} else if(role == Qt::DisplayRole) {
			//fetch the id data, to make shure the models data is loaded!
			auto sIndex = mapToSource(index);
			sourceModel()->data(sIndex, Qt::DisplayRole);
		}
		break;
	case 1:
		if(role == Qt::DecorationRole) {
			switch(data.toInt()) {
			case 0:
				return QIcon(QStringLiteral(":/icons/empty.ico"));
			case 1:
				return QIcon::fromTheme(QStringLiteral("media-playlist-repeat"), QIcon(QStringLiteral(":/icons/loop.ico")));
			case 2:
				return QIcon::fromTheme(QStringLiteral("alarm-symbolic"), QIcon(QStringLiteral(":/icons/snooze.ico")));
			case 3:
				return QIcon::fromTheme(QStringLiteral("view-calendar-upcoming-events"), QIcon(QStringLiteral(":/icons/trigger.ico")));
			default:
				break;
			}
		} else if(role == Qt::ToolTipRole) {
			auto dateTime = QObjectProxyModel::data(index, Qt::DisplayRole);
			auto baseStr = QLocale().toString(dateTime.toDateTime(), QLocale::LongFormat);
			switch(data.toInt()) {
			case 0:
				return baseStr;
			case 1:
				return baseStr.append(tr("\nReminder will repeatedly trigger, not only once"));
			case 2:
				return baseStr.append(tr("\nReminder has been snoozed until the displayed time"));
			case 3:
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

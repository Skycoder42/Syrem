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

	connect(_ui->action_Close, &QAction::triggered,
			this, &MainWindow::close);
	connect(_ui->action_Quit, &QAction::triggered,
			qApp, &QApplication::quit);
	connect(_ui->action_Settings, &QAction::triggered,
			_control, &MainControl::showSettings);
	connect(_ui->action_Add_Reminder, &QAction::triggered,
			_control, &MainControl::addReminder);

	_ui->treeView->addActions({
								  _ui->action_Add_Reminder,
								  _ui->action_Delete_Reminder
							  });

	auto remModel = _control->reminderModel();
	auto initFn = [this](){
		_proxyModel->setSourceModel(_control->reminderModel());

		_sortModel->setSourceModel(_proxyModel);
		_ui->treeView->setModel(_sortModel);

		_ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
		_ui->treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
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
	auto sIndex = _ui->treeView->currentIndex();
	if(!sIndex.isValid())
		return;

	auto pIndex = _sortModel->mapToSource(sIndex);
	if(!pIndex.isValid())
		return;

	auto index = _proxyModel->mapToSource(pIndex);
	if(!index.isValid())
		return;

	_control->removeReminder(index.row());
}

void MainWindow::on_action_About_triggered()
{
	DialogMaster::about(this,
						tr("A simple reminder application for desktop and mobile, with synchronized reminder."),
						QStringLiteral("https://github.com/Skycoder42/RemindMe"),
						tr("BSD 3 Clause"),
						QStringLiteral("https://github.com/Skycoder42/RemindMe/blob/master/LICENSE"));
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
		} else if(role == Qt::DisplayRole) {
			//fetch the id data, to make shure the models data is loaded!
			auto sIndex = mapToSource(index);
			sourceModel()->data(sIndex, Qt::DisplayRole);
		}
		break;
	case 1:
		if(role == Qt::DecorationRole) {
			auto decorData = QObjectProxyModel::data(index, Qt::ToolTipRole);
			if(decorData.toDateTime().isValid())
				return QIcon::fromTheme(QStringLiteral("alarm-symbolic"), QIcon(QStringLiteral(":/icons/snooze.ico")));
			else if(data.toBool())
				return QIcon::fromTheme(QStringLiteral("media-playlist-repeat"), QIcon(QStringLiteral(":/icons/snooze.ico")));
			else
				return QIcon(QStringLiteral(":/icons/empty.ico"));
		} else if(role == Qt::ToolTipRole) {
			auto toolData = QObjectProxyModel::data(index, Qt::DecorationRole);
			auto snooze = data.toDateTime();
			if(snooze.isValid())
				return tr("Reminder has been snoozed until the displayed time");
			else if(toolData.toBool())
				return tr("Reminder will repeatedly trigger, not only once");
			else
				return QVariant();
		} else if(role == Qt::DisplayRole) {
			auto dateTime = data.toDateTime();
			return QLocale().toString(dateTime, format);
		}
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
	addMapping(0, Qt::DisplayRole, "text");
	addMapping(1, Qt::DisplayRole, "current");
	addMapping(1, Qt::DecorationRole, "repeating");
	addMapping(1, Qt::ToolTipRole, "snooze");
}

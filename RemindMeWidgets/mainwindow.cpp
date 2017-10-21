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

	auto sep = new QAction(this);
	sep->setSeparator(true);
	_ui->treeView->addActions({
								  _ui->action_Add_Reminder,
								  sep,
								  _ui->actionEdit_Reminder,
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



ReminderProxyModel::ReminderProxyModel(QObject *parent) :
	QObjectProxyModel({tr("Reminder"), tr("Next trigger")}, parent),
	_settings(new QSettings(this))
{}

QVariant ReminderProxyModel::data(const QModelIndex &index, int role) const
{
	auto data = QObjectProxyModel::data(index, role);
	if(!data.isValid())
		return {};

	switch (index.column()) {
	case 0:
		if(role == Qt::DecorationRole) {
			auto important = data.toBool();
			if(important)
				return QIcon::fromTheme(QStringLiteral("emblem-important-symbolic"), QIcon(QStringLiteral(":/icons/important.ico")));
			else
				return QIcon(QStringLiteral(":/icons/empty.ico"));
		}
		break;
	case 1:
		if(role == Qt::DisplayRole) {
			auto dateTime = data.toDateTime();
			auto format = _settings->value(QStringLiteral("gui/dateformat"), QLocale::ShortFormat).toInt();
			return QLocale().toString(dateTime, (QLocale::FormatType)format);
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
}

void MainWindow::on_action_About_triggered()
{
	DialogMaster::about(this,
						tr("A simple reminder application for desktop and mobile, with synchronized reminder."),
						QStringLiteral("https://github.com/Skycoder42/RemindMe"),
						tr("BSD 3 Clause"),
						QStringLiteral("https://github.com/Skycoder42/RemindMe/blob/master/LICENSE"));
}

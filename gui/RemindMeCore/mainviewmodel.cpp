#include "mainviewmodel.h"

#include <QGuiApplication>
#include <QtMvvmCore/Messages>
#include <QtMvvmCore/SettingsViewModel>
#include <QtMvvmDataSyncCore/DataSyncViewModel>

#include "createreminderviewmodel.h"
#include "snoozeviewmodel.h"
#include "datasyncsettingsviewmodel.h"

namespace { //NOTE remove once fixed

class BugModel : public QtDataSync::DataStoreModel
{
public:
	inline BugModel(QObject *parent) :
		DataStoreModel{parent}
	{}

	// QAbstractItemModel interface
public:
	inline QHash<int, QByteArray> roleNames() const override {
		auto rNames = QtDataSync::DataStoreModel::roleNames();
		rNames.remove(0); //remove the original id
		rNames.insert(rNames.key("bugId"), "id"); //replace by bugId
		return rNames;
	}
};

}

const QString MainViewModel::paramRemId{QStringLiteral("reminder-id")};

MainViewModel::MainViewModel(QObject *parent) :
	ViewModel{parent},
	_reminderModel{new BugModel{this}},
	_sortedModel{new QSortFilterProxyModel{this}}
{
	static Q_CONSTEXPR auto SortRole = Qt::UserRole + 100;
	_reminderModel->setTypeId<Reminder>();
	auto column = _reminderModel->addColumn(tr("Reminder"), "description");
	_reminderModel->addRole(column, Qt::DecorationRole, "important");
	_reminderModel->addRole(column, Qt::ToolTipRole, "description");
	_reminderModel->addRole(column, SortRole, "description");
	column = _reminderModel->addColumn(tr("Due on"), "current");
	_reminderModel->addRole(column, Qt::DecorationRole, "triggerState");
	_reminderModel->addRole(column, Qt::ToolTipRole, "triggerState");
	_reminderModel->addRole(column, SortRole, "current");

	_sortedModel->setSortLocaleAware(true);
	_sortedModel->setSourceModel(_reminderModel);
	_sortedModel->setSortRole(SortRole);
	_sortedModel->sort(1);

	connect(_reminderModel, &QtDataSync::DataStoreModel::storeError,
			this, [](const QException &error){
		qCritical() << error.what();
	});
}

QVariantHash MainViewModel::showParams(const QUuid &reminderId)
{
	return {
		{paramRemId, reminderId}
	};
}

QtDataSync::DataStoreModel *MainViewModel::reminderModel() const
{
	return _reminderModel;
}

QSortFilterProxyModel *MainViewModel::sortedModel() const
{
	return _sortedModel;
}

void MainViewModel::showSettings()
{
	show<QtMvvm::SettingsViewModel>(QtMvvm::SettingsViewModel::showParams(_settings->accessor()));
}

void MainViewModel::showSync()
{
	show<QtMvvm::DataSyncViewModel>();
}

void MainViewModel::showAbout()
{
	QtMvvm::about(tr("A simple reminder application for desktop and mobile, with synchronized reminder."),
				  QStringLiteral("https://github.com/Skycoder42/RemindMe"),
				  tr("BSD 3 Clause"),
				  QStringLiteral("https://github.com/Skycoder42/RemindMe/blob/master/LICENSE"));
}

void MainViewModel::addReminder()
{
	show<CreateReminderViewModel>();
}

void MainViewModel::completeReminder(QUuid id)
{
	try {
		auto rem = _reminderModel->store()->load<Reminder>(id);
		rem.nextSchedule(_reminderModel->store(), QDateTime::currentDateTime());
		if(_settings->scheduler.urlOpen)
			rem.openUrls();
	} catch(QtDataSync::NoDataException &e) {
		qDebug() << "Skipping completing of deleted reminder" << id
				 << "with reason" << e.what();
	} catch(QException &e) {
		qCritical() << "Failed to complete reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to complete reminder"),
						 tr("Reminder could not be move to the next state. It has most likely become corrupted."));
	}
}

void MainViewModel::deleteReminder(QUuid id)
{
	try {
		if(!_reminderModel->store()->remove<Reminder>(id))
			qWarning() << "Reminder with id" << id << "has already been removed";
	} catch(QException &e) {
		qCritical() << "Failed to remove reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to remove reminder"),
						 tr("Reminder could not be deleted from datastore!"));
	}
}

void MainViewModel::snoozeReminder(QUuid id)
{
	if(id.isNull())
		return;

	try {
		auto rem = _reminderModel->store()->load<Reminder>(id);
		if(rem.triggerState() == Reminder::Triggered)
			show<SnoozeViewModel>(SnoozeViewModel::showParams(rem));
	} catch(QtDataSync::NoDataException &e) {
		qDebug() << "Skipping completing of deleted reminder" << id
				 << "with reason" << e.what();
	} catch (QException &e) {
		qCritical() << "Failed to load reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to snooze reminder"),
						 tr("Unable to load the reminder that should be snoozed!"));
	}
}

void MainViewModel::openReminderUrls(QUuid id)
{
	if(id.isNull())
		return;

	try {
		auto rem = _reminderModel->store()->load<Reminder>(id);
		rem.openUrls();
	} catch (QException &e) {
		qCritical() << "Failed to load reminder with error:" << e.what();
		QtMvvm::critical(tr("Failed to open reminder urls"),
						 tr("Unable to load the reminder to open its urls!"));
	}
}

void MainViewModel::onInit(const QVariantHash &params)
{
	QGuiApplication::setQuitOnLastWindowClosed(true);
	if(params.contains(paramRemId)) {
		auto remId = params.value(paramRemId).toUuid();
		auto mIndex = _reminderModel->idIndex(remId);
		if(mIndex.isValid())
			emit select(mIndex.row());
	}
}

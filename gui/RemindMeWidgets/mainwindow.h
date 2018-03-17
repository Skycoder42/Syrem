#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QSettings>

#include <qobjectproxymodel.h>
#include <mainviewmodel.h>

namespace Ui {
class MainWindow;
}

class ReminderProxyModel : public QObjectProxyModel
{
	Q_OBJECT

public:
	ReminderProxyModel(QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role) const override;
	void setSourceModel(QAbstractItemModel *sourceModel) override;

private:
	QSettings *_settings;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	Q_INVOKABLE MainWindow(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_action_Complete_Reminder_triggered();
	void on_action_Delete_Reminder_triggered();
	void on_action_Snooze_Reminder_triggered();

	void on_treeView_activated(const QModelIndex &index);
	void updateCurrent(const QModelIndex &index);

private:
	MainViewModel *_viewModel;
	Ui::MainWindow *_ui;

	ReminderProxyModel *_proxyModel;
	QSortFilterProxyModel *_sortModel;

	Reminder reminderAt(const QModelIndex &sIndex);
};

#endif // MAINWINDOW_H

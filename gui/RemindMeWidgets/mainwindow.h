#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIdentityProxyModel>
#include <mainviewmodel.h>

namespace Ui {
class MainWindow;
}

class ReminderProxyModel : public QIdentityProxyModel
{
	Q_OBJECT

public:
	ReminderProxyModel(QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role) const override;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	Q_INVOKABLE MainWindow(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void select(int row);

	void on_action_Complete_Reminder_triggered();
	void on_action_Delete_Reminder_triggered();
	void on_action_Snooze_Reminder_triggered();

	void on_treeView_activated(const QModelIndex &index);
	void updateCurrent(const QModelIndex &index);

private:
	MainViewModel *_viewModel;
	Ui::MainWindow *_ui;

	ReminderProxyModel *_proxyModel;

	Reminder reminderAt(const QModelIndex &sIndex);
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <mainviewmodel.h>

namespace Ui {
class MainWindow;
}

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

	QUuid idFromIndex(const QModelIndex &index);
};

#endif // MAINWINDOW_H

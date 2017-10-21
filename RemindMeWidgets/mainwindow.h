#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <qobjectproxymodel.h>
#include <maincontrol.h>

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

};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	Q_INVOKABLE MainWindow(Control *mControl, QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_action_About_triggered();

private:
	MainControl *_control;
	Ui::MainWindow *_ui;

	ReminderProxyModel *_proxyModel;
	QSortFilterProxyModel *_sortModel;
};

#endif // MAINWINDOW_H

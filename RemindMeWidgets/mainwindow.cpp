#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qtmvvmbinding.h>

MainWindow::MainWindow(Control *mControl, QWidget *parent) :
	QMainWindow(parent),
	control(static_cast<MainControl*>(mControl)),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->treeView->setModel(control->reminderModel());
}

MainWindow::~MainWindow()
{
	delete ui;
}

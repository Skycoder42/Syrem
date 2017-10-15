#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qtmvvmbinding.h>

MainWindow::MainWindow(Control *mControl, QWidget *parent) :
	QMainWindow(parent),
	control(static_cast<MainControl*>(mControl)),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QtMvvmBinding::bind(control, "text", ui->lineEdit, "text");
	QtMvvmBinding::bind(control, "text", ui->label, "text", QtMvvmBinding::OneWayFromControl);
}

MainWindow::~MainWindow()
{
	delete ui;
}

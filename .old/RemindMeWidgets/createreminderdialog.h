#ifndef CREATEREMINDERDIALOG_H
#define CREATEREMINDERDIALOG_H

#include <QDialog>
#include <createreminderviewmodel.h>

namespace Ui {
class CreateReminderDialog;
}

class CreateReminderDialog : public QDialog
{
	Q_OBJECT

public:
	Q_INVOKABLE CreateReminderDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~CreateReminderDialog();

public slots:
	void accept() override;

private slots:
	void created(bool success);

	void on_actionExpression_Syntax_triggered();

private:
	CreateReminderViewModel *_viewModel;
	Ui::CreateReminderDialog *_ui;
};

#endif // CREATEREMINDERDIALOG_H

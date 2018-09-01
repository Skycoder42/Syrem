#ifndef WIDGETSSNOOZEDIALOG_H
#define WIDGETSSNOOZEDIALOG_H

#include <QDialog>
#include <QToolBox>
#include "traysnoozeviewmodel.h"

class TraySnoozeDialog : public QDialog
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit TraySnoozeDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);

public slots:
	void reject() override;

private slots:
	void performComplete();
	void performSnooze();
	void performUrlOpen();

	void reloadReminders(const QList<Reminder> &reminders);

private:
	TraySnoozeViewModel *_viewModel;
	QToolBox *_toolBox = nullptr;
	QHash<QWidget*, QUuid> _reminders;

	void setupUi();
	void resizeUi();
	void addReminder(const Reminder &reminder);
};

#endif // WIDGETSSNOOZEDIALOG_H

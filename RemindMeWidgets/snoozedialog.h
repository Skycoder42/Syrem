#ifndef SNOOZEDIALOG_H
#define SNOOZEDIALOG_H

#include <QDateTime>
#include <QInputDialog>
#include <QSettings>
#include <snoozecontrol.h>

class SnoozeDialog : public QInputDialog
{
	Q_OBJECT

public:
	Q_INVOKABLE SnoozeDialog(Control *control, QWidget *parent = nullptr);

	void accept() override;

private:
	SnoozeControl *_control;
};

#endif // SNOOZEDIALOG_H

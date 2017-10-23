#ifndef SNOOZEDIALOG_H
#define SNOOZEDIALOG_H

#include <QDateTime>
#include <QDialog>
#include <dateparser.h> //direct use ok here, as it's part of a daemon service...

namespace Ui {
class SnoozeDialog;
}

class SnoozeDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SnoozeDialog(QWidget *parent = nullptr);
	~SnoozeDialog();

	QDateTime snoozeTime() const;

	void accept() override;

private:
	Ui::SnoozeDialog *_ui;
	DateParser *_parser;
	QDateTime _nextTime;
};

#endif // SNOOZEDIALOG_H

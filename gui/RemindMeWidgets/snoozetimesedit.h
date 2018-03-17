#ifndef SNOOZETIMESEDIT_H
#define SNOOZETIMESEDIT_H

#include <QVariantList>
#include <QWidget>
#include <snoozetimes.h>

namespace Ui {
class SnoozeTimesEdit;
}

class SnoozeTimesEdit : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(SnoozeTimes times READ times WRITE setTimes USER true)

public:
	explicit SnoozeTimesEdit(QWidget *parent = nullptr);
	~SnoozeTimesEdit();

	SnoozeTimes times() const;

public slots:
	void setTimes(const SnoozeTimes &times);

private slots:
	void on_action_Add_Time_triggered();
	void on_action_Remove_Time_triggered();
	void on_actionMove_Up_triggered();
	void on_actionMove_Down_triggered();

private:
	Ui::SnoozeTimesEdit *_ui;
};

#endif // SNOOZETIMESEDIT_H

#ifndef SNOOZEDIALOG_H
#define SNOOZEDIALOG_H

#include <QDateTime>
#include <QInputDialog>
#include <QSettings>
#include <snoozecontrol.h>

class SnoozeDialog : public QInputDialog
{
	Q_OBJECT

	Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)
	Q_PROPERTY(QStringList comboBoxItems READ comboBoxItems WRITE setComboBoxItems)
	Q_PROPERTY(QString textValue READ textValue WRITE setTextValue)

public:
	Q_INVOKABLE SnoozeDialog(Control *control, QWidget *parent = nullptr);

	void accept() override;

private:
	SnoozeControl *_control;
	QString _title;
};

#endif // SNOOZEDIALOG_H

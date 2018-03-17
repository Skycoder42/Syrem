#ifndef SNOOZEDIALOG_H
#define SNOOZEDIALOG_H

#include <QDateTime>
#include <QInputDialog>
#include <QSettings>
#include <snoozeviewmodel.h>

class SnoozeDialog : public QInputDialog
{
	Q_OBJECT

	Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)
	Q_PROPERTY(QStringList comboBoxItems READ comboBoxItems WRITE setComboBoxItems)
	Q_PROPERTY(QString textValue READ textValue WRITE setTextValue)

public:
	Q_INVOKABLE SnoozeDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);

	void accept() override;

private:
	SnoozeViewModel *_viewModel;
	QString _title;
};

#endif // SNOOZEDIALOG_H

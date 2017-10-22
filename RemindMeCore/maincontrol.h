#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include <control.h>
#include <QAbstractItemModelReplica>
#include <QRemoteObjectHost>
class ReminderManagerReplica;

class MainControl : public Control
{
	Q_OBJECT

	Q_PROPERTY(QAbstractItemModelReplica* reminderModel READ reminderModel CONSTANT)

public:
	explicit MainControl(QObject *parent = nullptr);

	void onShow() override;
	void onClose() override;

	QAbstractItemModelReplica* reminderModel() const;

public slots:
	void showSettings();
	void addReminder();
	void removeReminder(int index);

private:
	QRemoteObjectNode *_node;
	ReminderManagerReplica *_reminderManager;
	QAbstractItemModelReplica* _reminderModel;
};

#endif // MAINCONTROL_H

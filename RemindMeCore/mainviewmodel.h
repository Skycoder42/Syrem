#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include <QtMvvmCore/ViewModel>
#include <QAbstractItemModelReplica>
#include <QRemoteObjectHost>
#include <QUuid>
class ReminderManagerReplica;

class MainViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QAbstractItemModelReplica* reminderModel READ reminderModel CONSTANT)

public:
	Q_INVOKABLE explicit MainViewModel(QObject *parent = nullptr);

	QAbstractItemModelReplica* reminderModel() const;

public slots:
	void showSettings();

	void addReminder();
	void removeReminder(const QUuid &id);
	void snoozeReminder(const QUuid &id, quint32 versionCode);

private slots:
	void reminderError(bool isCreate, const QString &error);

private:
	QRemoteObjectNode *_node;
	ReminderManagerReplica *_reminderManager;
	QAbstractItemModelReplica* _reminderModel;
};

#endif // MAINCONTROL_H

#ifndef SNOOZECONTROL_H
#define SNOOZECONTROL_H

#include <QObject>
#include <QUuid>
#include <control.h>

class SnoozeControl : public Control
{
	Q_OBJECT

	Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
	Q_PROPERTY(QStringList snoozeTimes READ snoozeTimes NOTIFY snoozeTimesChanged)

public:
	explicit SnoozeControl(QObject *parent = nullptr);

	QString description() const;
	QStringList snoozeTimes() const;

public slots:
	void show(const QUuid &id);

	void snooze();

signals:
	void descriptionChanged(QString description);
	void snoozeTimesChanged(QStringList snoozeTimes);

private:
	QUuid _reminderId;
	QString _description;
	QStringList _snoozeTimes;
};

#endif // SNOOZECONTROL_H

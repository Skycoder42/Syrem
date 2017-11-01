#ifndef SNOOZECONTROL_H
#define SNOOZECONTROL_H

#include <QObject>
#include <QUuid>
#include <control.h>
class SnoozeHelperReplica;

class SnoozeControl : public Control
{
	Q_OBJECT

	Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
	Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
	Q_PROPERTY(QStringList snoozeTimes READ snoozeTimes NOTIFY snoozeTimesChanged)
	Q_PROPERTY(QString expression READ expression WRITE setExpression NOTIFY expressionChanged)

public:
	explicit SnoozeControl(QObject *parent = nullptr);

	bool isLoaded() const;
	QString description() const;
	QStringList snoozeTimes() const;
	QString expression() const;

public slots:
	void show(const QUuid &id, quint32 versionCode);
	void snooze();

	void setExpression(QString expression);

signals:
	void loadedChanged(bool loaded);
	void descriptionChanged(QString description);
	void snoozeTimesChanged(QStringList snoozeTimes);
	void expressionChanged(QString expression);

private slots:
	void reminderLoaded(const QUuid &id, const QString &description);
	void reminderSnoozed(const QUuid &id);
	void reminderError(const QUuid &id, const QString &error, bool close);

private:
	SnoozeHelperReplica *_snoozeHelper;
	bool _loaded;

	QUuid _reminderId;
	QString _description;
	QStringList _snoozeTimes;
	QString _expression;
};

#endif // SNOOZECONTROL_H

#ifndef SNOOZECONTROL_H
#define SNOOZECONTROL_H

#include <QObject>
#include <QUuid>
#include <QtMvvmCore/ViewModel>
#include <QtMvvmCore/Injection>
#include <remindmelib.h>
#include <dateparser.h>
#include <syncedsettings.h>

class SnoozeViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(bool valid READ isValid NOTIFY reminderLoaded)
	Q_PROPERTY(QString description READ description NOTIFY reminderLoaded)
	Q_PROPERTY(QStringList snoozeTimes READ snoozeTimes NOTIFY reminderLoaded)
	Q_PROPERTY(QString expression READ expression WRITE setExpression NOTIFY expressionChanged)

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)
	QTMVVM_INJECT_PROP(DateParser*, parser, _parser)

public:
	const static QString paramReminder;

	static QVariantHash showParams(const Reminder &reminder);

	Q_INVOKABLE explicit SnoozeViewModel(QObject *parent = nullptr);

	bool isValid() const;
	QString description() const;
	QStringList snoozeTimes() const;
	QString expression() const;

	Q_INVOKABLE bool snooze();

public slots:
	void setExpression(const QString &expression);

signals:
	void reminderLoaded();
	void expressionChanged(const QString &expression);

protected:
	void onInit(const QVariantHash &params) override;

private:
	SyncedSettings *_settings;
	DateParser *_parser;
	ReminderStore *_store;

	Reminder _reminder;
	QStringList _snoozeTimes;
	QString _expression;
};

#endif // SNOOZECONTROL_H

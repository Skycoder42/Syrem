#ifndef SNOOZECONTROL_H
#define SNOOZECONTROL_H

#include <QObject>
#include <QUuid>
#include <QtMvvmCore/ViewModel>
#include <QtMvvmCore/Injection>
#include <libsyrem.h>
#include <eventexpressionparser.h>
#include <syncedsettings.h>

class SnoozeViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(bool valid READ isValid NOTIFY reminderLoaded)
	Q_PROPERTY(QString description READ description NOTIFY reminderLoaded)
	Q_PROPERTY(QStringList snoozeTimes READ snoozeTimes NOTIFY reminderLoaded)
	Q_PROPERTY(QString expression READ expression WRITE setExpression NOTIFY expressionChanged)
	Q_PROPERTY(bool blocked READ isBlocked NOTIFY blockedChanged)

	Q_PROPERTY(Reminder reminder READ reminder NOTIFY reminderLoaded)

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)
	QTMVVM_INJECT_PROP(EventExpressionParser*, parser, _parser)

public:
	const static QString paramReminder;
	const static QString paramReminderId;

	static QVariantHash showParams(const Reminder &reminder);
	static QVariantHash showParams(QUuid reminderId);

	Q_INVOKABLE explicit SnoozeViewModel(QObject *parent = nullptr);

	bool isValid() const;
	QString description() const;
	QStringList snoozeTimes() const;
	QString expression() const;
	bool isBlocked() const;
	Reminder reminder() const;

public slots:
	void snooze();
	void setExpression(const QString &expression);

signals:
	void reminderLoaded();
	void close();

	void expressionChanged(const QString &expression);
	void blockedChanged(bool blocked);

protected:
	void onInit(const QVariantHash &params) override;
	void onResult(quint32 requestCode, const QVariant &result) override;

private:
	static constexpr int TermSelectCode = 20;

	SyncedSettings *_settings = nullptr;
	EventExpressionParser *_parser = nullptr;
	ReminderStore *_store;

	Reminder _reminder;
	QStringList _snoozeTimes;
	QString _expression;
	bool _blocked = false;

	void finishSnooze(const Expressions::Term &term);
	void setBlocked(bool blocked);
};

#endif // SNOOZECONTROL_H

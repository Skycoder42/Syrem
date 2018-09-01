#ifndef WIDGETSSNOOZEVIEWMODEL_H
#define WIDGETSSNOOZEVIEWMODEL_H

#include <QtMvvmCore/ViewModel>
#include <syncedsettings.h>
#include <eventexpressionparser.h>
#include <reminder.h>
#include "inotifier.h"

class TraySnoozeViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QList<Reminder> reminders READ reminders NOTIFY remindersChanged)
	Q_PROPERTY(bool blocked READ isBlocked NOTIFY blockedChanged)

	QTMVVM_INJECT_PROP(EventExpressionParser*, parser, _parser)
	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	static const QString paramNotifier;
	static const QString paramReminders;
	static QVariantHash showParams(INotifier *notifier, const QList<Reminder> &reminders);

	Q_INVOKABLE explicit TraySnoozeViewModel(QObject *parent = nullptr);

	QList<Reminder> reminders() const;
	SyncedSettings *settings() const;
	bool isBlocked() const;

public slots:
	void performComplete(QUuid id);
	void performSnooze(QUuid id, const QString &expression);
	void openUrls(QUuid id);

signals:
	void remindersChanged(const QList<Reminder> &reminders);
	void blockedChanged(bool blocked);

protected:
	void onInit(const QVariantHash &params) override;
	void onResult(quint32 requestCode, const QVariant &result) override;

private:
	static constexpr int TermSelectCode = 30;

	EventExpressionParser *_parser = nullptr;
	SyncedSettings *_settings = nullptr;

	INotifier *_notifier = nullptr;
	QHash<QUuid, Reminder> _reminders;
	quint32 _requestCounter = TermSelectCode;
	QHash<quint32, QUuid> _requests;
	bool _blocked = false;

	void finishSnooze(QUuid id, const Expressions::Term &term);
	void setBlocked(bool blocked);
};

#endif // WIDGETSSNOOZEVIEWMODEL_H

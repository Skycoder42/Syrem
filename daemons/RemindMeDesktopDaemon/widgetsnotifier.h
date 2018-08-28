#ifndef WIDGETSNOTIFIER_H
#define WIDGETSNOTIFIER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QMenu>
#include <QtMvvmCore/Injection>
#include <syncedsettings.h>
#include <eventexpressionparser.h>
#include "inotifier.h"
#include "widgetssnoozedialog.h"

class WidgetsNotifier : public QObject, public INotifier
{
	Q_OBJECT
	Q_INTERFACES(INotifier)

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)
	QTMVVM_INJECT_PROP(EventExpressionParser*, parser, _parser)

public:
	Q_INVOKABLE explicit WidgetsNotifier(QObject *parent = nullptr);

public slots:
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;
	void showErrorMessage(const QString &error) override;
	void cancelAll() override;

signals:
	void messageCompleted(const QUuid &id, quint32 versionCode) final;
	void messageDelayed(const QUuid &id, quint32 versionCode, const QDateTime &nextTrigger) final;
	void messageActivated(const QUuid &id = {}) final;

private slots:
	void qtmvvm_init();
	void activated(QSystemTrayIcon::ActivationReason reason);
	void trigger();
	void dismiss();
	void showMainApp();
	void invert();

	void snoozeAction(Reminder reminder, bool completed, const QDateTime &snoozeTime);
	void snoozeDone(const QList<Reminder> &reminders);

private:
	const QIcon _normalIcon;
	const QIcon _inverseIcon;
	const QIcon _errorIcon;

	SyncedSettings *_settings;
	EventExpressionParser *_parser;
	QSystemTrayIcon *_trayIco;
	QMenu *_trayMenu;
	QTimer *_blinkTimer;
	bool _inverted;

	QHash<QUuid, Reminder> _notifications;
	QString _lastError;

	void updateIcon();
};

#endif // WIDGETSNOTIFIER_H

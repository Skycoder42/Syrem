#ifndef WIDGETSNOTIFIER_H
#define WIDGETSNOTIFIER_H

#include <QObject>
#include <inotifier.h>
#include <QSystemTrayIcon>
#include <qtaskbarcontrol.h>
#include <QTimer>
#include "widgetssnoozedialog.h"

class WidgetsNotifier : public QObject, public INotifier
{
	Q_OBJECT
	Q_INTERFACES(INotifier)

public:
	explicit WidgetsNotifier(QObject *parent = nullptr);

public slots:
	void setupEmtpy() override;
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;
	void showErrorMessage(const QString &error) override;

signals:
	void messageDismissed(Reminder reminder) final;
	void messageCompleted(Reminder reminder) final;
	void messageDelayed(Reminder reminder, const QDateTime &nextTrigger) final;

private slots:
	void activated(QSystemTrayIcon::ActivationReason reason);
	void invert();

	void snoozeAction(Reminder reminder, WidgetsSnoozeDialog::Action action, const QDateTime &snoozeTime);
	void snoozeAborted(const QList<Reminder> &reminders);

private:
	const QIcon _normalIcon;
	const QIcon _inverseIcon;
	const QIcon _errorIcon;

	QSystemTrayIcon *_trayIco;
	QTaskbarControl *_taskbar;
	QTimer *_blinkTimer;
	bool _inverted;

	QHash<QUuid, Reminder> _notifications;
	QString _lastError;

	void updateIcon();
};

#endif // WIDGETSNOTIFIER_H

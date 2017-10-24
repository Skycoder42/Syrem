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

signals:
	void messageDismissed(const QUuid &id) final;
	void messageCompleted(const QUuid &id) final;
	void messageDelayed(const QUuid &id, const QDateTime &nextTrigger) final;

private slots:
	void activated(QSystemTrayIcon::ActivationReason reason);
	void invert();

	void snoozeAction(const QUuid &id, WidgetsSnoozeDialog::Action action, const QDateTime &snoozeTime);
	void snoozeAborted(const QList<Reminder> &reminders);

private:
	const QIcon _normalIcon;
	const QIcon _inverseIcon;

	QSystemTrayIcon *_trayIco;
	QTaskbarControl *_taskbar;
	QTimer *_blinkTimer;
	bool _inverted;

	QHash<QUuid, Reminder> _notifications;

	void updateIcon();
};

#endif // WIDGETSNOTIFIER_H

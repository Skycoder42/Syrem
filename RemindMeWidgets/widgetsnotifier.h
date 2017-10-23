#ifndef WIDGETSNOTIFIER_H
#define WIDGETSNOTIFIER_H

#include <QObject>
#include <inotifier.h>
#include <QSystemTrayIcon>
#include <qtaskbarcontrol.h>

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

private:
	QSystemTrayIcon *_trayIco;
	QTaskbarControl *_taskbar;

	QHash<QUuid, Reminder> _notifications;
};

#endif // WIDGETSNOTIFIER_H

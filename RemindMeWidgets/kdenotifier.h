#ifndef KDENOTIFIER_H
#define KDENOTIFIER_H

#include <QObject>
#include <KStatusNotifierItem>
#include <KNotification>
#include <inotifier.h>
#include <qtaskbarcontrol.h>

class KdeNotifier : public QObject, public INotifier
{
	Q_OBJECT
	Q_INTERFACES(INotifier)

public:
	explicit KdeNotifier(QObject *parent = nullptr);

public slots:
	void setupEmtpy() override;
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;

signals:
	void messageDismissed(const QUuid &id) final;
	void messageCompleted(const QUuid &id) final;
	void messageDelayed(const QUuid &id, const QDateTime &nextTrigger) final;

private slots:
	void snoozed(const QUuid &id, bool defaultSnooze);

private:
	typedef QPair<Reminder, KNotification*> NotifyInfo;

	KStatusNotifierItem *_statusNotifier;
	QTaskbarControl *_taskbar;

	QHash<QUuid, NotifyInfo> _notifications;

	void updateIcon();
	bool removeNot(const QUuid &id, bool close = false, Reminder *remPtr = nullptr);
};

#endif // KDENOTIFIER_H

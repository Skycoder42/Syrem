#ifndef KDENOTIFIER_H
#define KDENOTIFIER_H

#include <QObject>
#include <KNotification>
#include <inotifier.h>
#include <qtaskbarcontrol.h>
#include <QSettings>

class KdeNotifier : public QObject, public INotifier
{
	Q_OBJECT
	Q_INTERFACES(INotifier)

public:
	explicit KdeNotifier(QObject *parent = nullptr);

public slots:
	void beginSetup() override;
	void endSetup() override;
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;
	void showErrorMessage(const QString &error) override;

signals:
	void messageDismissed(Reminder reminder) final;
	void messageCompleted(Reminder reminder) final;
	void messageDelayed(Reminder reminder, const QDateTime &nextTrigger) final;

private slots:
	void snoozed(const QUuid &id);

private:
	typedef QPair<Reminder, KNotification*> NotifyInfo;

	QTaskbarControl *_taskbar;
	QSettings *_settings;
	QHash<QUuid, NotifyInfo> _notifications;

	void updateBar();
	bool removeNot(const QUuid &id, Reminder *remPtr = nullptr, bool close = false);
};

#endif // KDENOTIFIER_H

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
	void notificationHandled(const QUuid &id, const QString &errorMsg) override;

signals:
	void messageDismissed(const QUuid &id, quint32 versionCode) final;
	void messageCompleted(const QUuid &id, quint32 versionCode) final;
	void messageDelayed(const QUuid &id, quint32 versionCode, const QDateTime &nextTrigger) final;

private:
	QTaskbarControl *_taskbar;
	QSettings *_settings;
	QHash<QUuid, KNotification*> _notifications;

	void updateBar();
	bool removeNot(const QUuid &id, bool close = false);
};

#endif // KDENOTIFIER_H

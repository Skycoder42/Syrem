#ifndef KDENOTIFIER_H
#define KDENOTIFIER_H

#include <QObject>
#include <QtMvvmCore/Injection>
#include <KNotification>
#include <inotifier.h>
#include <syncedsettings.h>

class KdeNotifier : public QObject, public INotifier
{
	Q_OBJECT
	Q_INTERFACES(INotifier)

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	Q_INVOKABLE explicit KdeNotifier(QObject *parent = nullptr);

public slots:
	void showNotification(const Reminder &reminder) override;
	void removeNotification(const QUuid &id) override;
	void showErrorMessage(const QString &error) override;

signals:
	void messageCompleted(const QUuid &id, quint32 versionCode) final;
	void messageDelayed(const QUuid &id, quint32 versionCode, const QDateTime &nextTrigger) final;
	void messageActivated(const QUuid &id) final;

private slots:
	void qtmvvm_init();

private:
	SyncedSettings *_settings;
	QHash<QUuid, KNotification*> _notifications;

	bool removeNot(const QUuid &id, bool close = false);
};

#endif // KDENOTIFIER_H

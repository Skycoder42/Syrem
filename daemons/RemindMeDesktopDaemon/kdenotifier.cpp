#include "kdenotifier.h"

#include <QApplication>
#include <dialogmaster.h>
#include "kdesnoozedialog.h"

#ifndef QT_NO_DEBUG
#include <QIcon>
#define Icon QIcon(QStringLiteral(":/icons/tray/main.ico")).pixmap(64, 64)
#define ErrorIcon QIcon(QStringLiteral(":/icons/tray/error.ico")).pixmap(64, 64)
#define setNotifyIcon setPixmap
#else
#define Icon QStringLiteral("remind-me")
#define ErrorIcon QStringLiteral("remind-me-error")
#define setNotifyIcon setIconName
#endif

KdeNotifier::KdeNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_settings(nullptr),
	_parser(nullptr),
	_notifications()
{}

void KdeNotifier::showNotification(const Reminder &reminder)
{
	auto important = reminder.isImportant();
	auto notification = new KNotification(important ?
											  QStringLiteral("remindimportant") :
											  QStringLiteral("remindnormal"),
										  KNotification::Persistent | KNotification::SkipGrouping,
										  this);
	_notifications.insert(reminder.id(), notification);

	notification->setTitle((important ?
							   tr("%1 — Important Reminder") :
							   tr("%1 — Reminder"))
						   .arg(QApplication::applicationDisplayName()));
	notification->setText(reminder.description());
	notification->setNotifyIcon(Icon);
	notification->setDefaultAction(tr("Open GUI"));
	notification->setActions({
								 tr("Complete"),
								 tr("Snooze")
							 });
	if(important)
		notification->setFlags(notification->flags() | KNotification::LoopSound);

	auto remId = reminder.id();
	auto vCode = reminder.versionCode();
	auto description = reminder.description();
	connect(notification, QOverload<>::of(&KNotification::activated), this, [this, remId](){
		if(removeNot(remId))
			emit messageActivated(remId);
	});
	connect(notification, &KNotification::action1Activated, this, [this, remId, vCode](){
		if(removeNot(remId))
			emit messageCompleted(remId, vCode);
	});
	connect(notification, &KNotification::action2Activated, this, [this, remId, vCode, description](){
		if(removeNot(remId)) {
			auto dialog = new KdeSnoozeDialog(_settings, _parser, description);
			connect(dialog, &KdeSnoozeDialog::timeSelected,
					this, [this, remId, vCode](const QDateTime &time){
				emit messageDelayed(remId, vCode, time);
			});
			dialog->open();
		}
	});
	connect(notification, &KNotification::closed, this, [this, remId, vCode](){
		removeNot(remId);
	});

	notification->sendEvent();
}

void KdeNotifier::removeNotification(const QUuid &id)
{
	removeNot(id, true);
}

void KdeNotifier::showErrorMessage(const QString &error)
{
	auto notification = new KNotification(QStringLiteral("error"),
										  KNotification::Persistent | KNotification::SkipGrouping,
										  this);

	notification->setTitle(tr("%1 — Error").arg(QApplication::applicationDisplayName()));
	notification->setText(error);
	notification->setNotifyIcon(ErrorIcon);
	connect(notification, &KNotification::closed, this, [notification]() {
		notification->deleteLater();
	});
	connect(qApp, &QApplication::aboutToQuit,
			notification, &KNotification::close);

	notification->sendEvent();
}

void KdeNotifier::qtmvvm_init()
{
	connect(qApp, &QApplication::aboutToQuit, this, [this](){
		for(auto notification : _notifications)
			notification->close();
	});
}

bool KdeNotifier::removeNot(const QUuid &id, bool close)
{
	auto notification = _notifications.take(id);
	if(notification) {
		if(close)
			notification->close();
		notification->deleteLater();
		return true;
	} else
		return false;
}

#include "kdenotifier.h"

#include <QApplication>
#include <dialogmaster.h>
#include "remindmeapp.h"

#ifndef QT_NO_DEBUG
#include <QIcon>
#include <snoozecontrol.h>
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
	_taskbar(new QTaskbarControl(new QWidget())), //create with a dummy widget parent
	_settings(new QSettings(this)),
	_notifications()
{
	connect(this, &KdeNotifier::destroyed,
			_taskbar->parent(), &QObject::deleteLater);
	_taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, QStringLiteral("remind-me.desktop"));

	connect(qApp, &QApplication::aboutToQuit, this, [this](){
		foreach(auto notification, _notifications)
			notification->close();
	});

	_settings->beginGroup(QStringLiteral("gui/notifications"));
}

void KdeNotifier::beginSetup() {}

void KdeNotifier::endSetup()
{
	updateBar();
}

void KdeNotifier::showNotification(const Reminder &reminder)
{
	auto important = reminder.isImportant();
	auto notification = new KNotification(important ?
											  QStringLiteral("remindimportant") :
											  QStringLiteral("remindnormal"),
										  KNotification::Persistent | KNotification::SkipGrouping,
										  this);

	_notifications.insert(reminder.id(), notification);
	updateBar();

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
	connect(notification, QOverload<>::of(&KNotification::activated), this, [](){
		coreApp->showMainControl();
	});
	connect(notification, &KNotification::action1Activated, this, [this, remId, vCode](){
		if(removeNot(remId))
			emit messageCompleted(remId, vCode);
	});
	connect(notification, &KNotification::action2Activated, this, [this, remId, vCode](){
		if(removeNot(remId))
			coreApp->showSnoozeControl(remId, vCode);
	});
	connect(notification, &KNotification::closed, this, [this, remId, vCode](){
		if(removeNot(remId))
			emit messageDismissed(remId, vCode);
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

void KdeNotifier::notificationHandled(const QUuid &id, const QString &errorMsg)
{
	removeNotification(id);
	if(!errorMsg.isNull())
		showErrorMessage(errorMsg);
}

void KdeNotifier::updateBar()
{
	_taskbar->setCounter(_notifications.size());
	if(_notifications.isEmpty())
		_taskbar->setCounterVisible(false);
	else {
		if(!_taskbar->counterVisible())
			_taskbar->setCounterVisible(true);
	}
}

bool KdeNotifier::removeNot(const QUuid &id, bool close)
{
	auto notification = _notifications.take(id);
	if(notification) {
		if(close)
			notification->close();
		notification->deleteLater();
		updateBar();
		return true;
	} else
		return false;
}

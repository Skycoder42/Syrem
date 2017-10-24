#include "kdenotifier.h"
#include "kdesnoozedialog.h"

#include <QApplication>
#include <dialogmaster.h>

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
	_taskbar(new QTaskbarControl(new QWidget())), //create with a dummy widget parent
	_notifications()
{
	connect(this, &KdeNotifier::destroyed,
			_taskbar->parent(), &QObject::deleteLater);
	_taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, QStringLiteral("remind-me.desktop"));

	connect(qApp, &QApplication::aboutToQuit, this, [this](){
		foreach(auto info, _notifications)
			info.second->close();
	});
}

void KdeNotifier::setupEmtpy()
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

	_notifications.insert(reminder.id(), {reminder, notification});
	updateBar();

	notification->setTitle((important ?
							   tr("%1 — Important Reminder") :
							   tr("%1 — Reminder"))
						   .arg(QApplication::applicationDisplayName()));
	notification->setText(reminder.text());
	notification->setNotifyIcon(Icon);
	notification->setActions({
								 tr("Complete"),
								 tr("Snooze")
							 });
	//TODO settings notification->setDefaultAction(notification->actions().last());
	if(important)
		notification->setFlags(notification->flags() | KNotification::LoopSound);

	auto remId = reminder.id();
	connect(notification, &KNotification::action1Activated, this, [this, remId](){
		if(removeNot(remId))
			emit messageCompleted(remId);
	});
	connect(notification, &KNotification::action2Activated, this, [this, remId](){
		snoozed(remId);
	});
	connect(notification, QOverload<>::of(&KNotification::activated), this, [this, remId](){
		//TODO settings...
	});
	connect(notification, &KNotification::closed, this, [this, remId](){
		if(removeNot(remId))
			emit messageDismissed(remId);
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

void KdeNotifier::snoozed(const QUuid &id)
{
	Reminder rem;
	if(!removeNot(id, false, &rem))
		return;

	auto diag = new KdeSnoozeDialog(rem.text() ,nullptr);

	connect(diag, &KdeSnoozeDialog::accepted, this, [this, id, diag]() {
		emit messageDelayed(id, diag->snoozeTime());
		diag->deleteLater();
	});
	connect(diag, &KdeSnoozeDialog::rejected, this, [this, id, diag]() {
		emit messageDismissed(id);
		diag->deleteLater();
	});

	diag->open();
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

bool KdeNotifier::removeNot(const QUuid &id, bool close, Reminder *remPtr)
{
	auto info = _notifications.take(id);
	if(info.second) {
		if(close)
			info.second->close();
		info.second->deleteLater();
		updateBar();

		if(remPtr)
			*remPtr = info.first;

		return true;
	} else
		return false;
}

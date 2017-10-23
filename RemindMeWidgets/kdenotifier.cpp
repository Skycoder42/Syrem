#include "kdenotifier.h"

#include <QApplication>

#ifndef QT_NO_DEBUG
#include <QIcon>
#define Icon QIcon(QStringLiteral(":/icons/tray/main.ico"))
#define IconPixmap Icon.pixmap(64, 64)
#define setIcon setIconByPixmap
#define setToolTipIcon setToolTipIconByPixmap
#define setNotifyIcon setPixmap
#else
#define Icon QStringLiteral("remind-me")
#define IconPixmap Icon
#define setIcon setIconByName
#define setToolTipIcon setToolTipIconByName
#define setNotifyIcon setIconName
#endif

KdeNotifier::KdeNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_statusNotifier(nullptr),
	_taskbar(new QTaskbarControl(new QWidget())), //create with a dummy widget parent
	_notifications()
{
	connect(this, &KdeNotifier::destroyed,
			_taskbar->parent(), &QObject::deleteLater);
	_taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, QStringLiteral("remind-me.desktop"));

	connect(qApp, &QApplication::aboutToQuit, this, [this](){
		foreach(auto info, _notifications)
			info.second->close();//TODO ??? -> or not?
	});
}

void KdeNotifier::setupEmtpy()
{
	_taskbar->setCounter(0);
	_taskbar->setCounterVisible(false);
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
	updateIcon();

	notification->setTitle(important ?
							   tr("Important Reminder triggered!") :
							   tr("Reminder triggered!"));
	notification->setText(reminder.text());
	notification->setNotifyIcon(IconPixmap);
	auto delayAct = tr("Delay…");
	notification->setActions({
								 tr("Complete"),
								 tr("Snooze"),
								 delayAct
							 });
	notification->setDefaultAction(delayAct);

	auto remId = reminder.id();
	connect(notification, &KNotification::action1Activated, this, [this, remId](){
		if(removeNot(remId))
			emit messageCompleted(remId);
	});
	connect(notification, &KNotification::action2Activated, this, [this, remId](){
		snoozed(remId, true);
	});
	connect(notification, &KNotification::action3Activated, this, [this, remId](){
		snoozed(remId, false);
	});
	connect(notification, &KNotification::closed, this, [this, remId](){
		if(removeNot(remId))
			emit messageDismissed(remId);
	});

	notification->sendEvent();
}

void KdeNotifier::removeNotification(const QUuid &id)
{
	if(removeNot(id, true))
		updateIcon();
}

void KdeNotifier::snoozed(const QUuid &id, bool defaultSnooze)
{

}

void KdeNotifier::updateIcon()
{
	_taskbar->setCounter(_notifications.size());
	if(_notifications.isEmpty()) {
		if(_statusNotifier) {
			_statusNotifier->deleteLater();
			_statusNotifier = nullptr;
		}
		_taskbar->setCounterVisible(false);
	} else {
		if(!_taskbar->counterVisible())
			_taskbar->setCounterVisible(true);

		if(!_statusNotifier) {
			_statusNotifier = new KStatusNotifierItem(this);
			_statusNotifier->setCategory(KStatusNotifierItem::Communications);
			_statusNotifier->setIcon(Icon);
			_statusNotifier->setStandardActionsEnabled(true);
			_statusNotifier->setTitle(QApplication::applicationDisplayName());
			_statusNotifier->setToolTipIcon(Icon);
			_statusNotifier->setToolTipTitle(QApplication::applicationDisplayName());
		}

		_statusNotifier->setToolTipSubTitle(tr("You have %n active reminder(s)", "", _notifications.size()));
		auto anyImportant = false;
		foreach(auto info, _notifications) {
			if(info.first.isImportant()) {
				anyImportant = true;
				break;
			}
		}

		if(anyImportant)
			_statusNotifier->setStatus(KStatusNotifierItem::NeedsAttention);
		else
			_statusNotifier->setStatus(KStatusNotifierItem::Active);
	}
}

bool KdeNotifier::removeNot(const QUuid &id, bool close)
{
	auto info = _notifications.take(id);
	if(info.second) {
		if(close)
			info.second->close();
		info.second->deleteLater();
		return true;
	} else
		return false;
}

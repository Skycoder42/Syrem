#include "kdenotifier.h"

#include <QApplication>

#ifndef QT_NO_DEBUG
#include <QIcon>
#define Icon QIcon(QStringLiteral(":/icons/tray/main.ico"))
#define setIcon setIconByPixmap
#define setToolTipIcon setToolTipIconByPixmap
#else
#define Icon QStringLiteral("remind-me")
#define setIcon setIconByName
#define setToolTipIcon setToolTipIconByName
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
}

void KdeNotifier::setupEmtpy()
{
	_taskbar->setCounter(0);
	_taskbar->setCounterVisible(false);
}

void KdeNotifier::showNotification(const Reminder &reminder)
{
	_notifications.insert(reminder.id(), reminder);
	updateIcon();

	_statusNotifier->showMessage(tr("Reminder triggered!"),
								 reminder.text(),
								 _statusNotifier->iconName(),
								 10000);
}

void KdeNotifier::removeNotification(const QUuid &id)
{
	if(_notifications.remove(id) > 0)
		updateIcon();
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
		foreach(auto rem, _notifications) {
			if(rem.isImportant()) {
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

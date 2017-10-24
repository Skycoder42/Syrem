#include "widgetsnotifier.h"

#include <QApplication>
#include <QTimer>

WidgetsNotifier::WidgetsNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_normalIcon(QStringLiteral(":/icons/tray/main.ico")),
	_inverseIcon(QStringLiteral(":/icons/tray/inverse.ico")),
	_trayIco(new QSystemTrayIcon(_normalIcon, this)),
	_taskbar(new QTaskbarControl(new QWidget())), //create with a dummy widget parent
	_blinkTimer(new QTimer(this)),
	_inverted(false),//true as default
	_notifications()
{
	connect(this, &WidgetsNotifier::destroyed,
			_taskbar->parent(), &QObject::deleteLater);
	_taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, QStringLiteral("remind-me.desktop"));

	_trayIco->setToolTip(QApplication::applicationDisplayName());
	connect(_trayIco, &QSystemTrayIcon::activated,
			this, &WidgetsNotifier::activated);
	connect(_trayIco, &QSystemTrayIcon::messageClicked,
			this, [this]() {
		activated(QSystemTrayIcon::Trigger);
	});

	_blinkTimer->setInterval(750);//TODO settings
	connect(_blinkTimer, &QTimer::timeout,
			this, &WidgetsNotifier::invert);
}

void WidgetsNotifier::setupEmtpy()
{
	updateIcon();
}

void WidgetsNotifier::showNotification(const Reminder &reminder)
{
	_notifications.insert(reminder.id(), reminder);
	updateIcon();

	_trayIco->showMessage(reminder.isImportant() ?
							  tr("Important Reminder triggered!") :
							  tr("Reminder triggered!"),
						  reminder.text(),
						  QSystemTrayIcon::Information,
						  10000);
}

void WidgetsNotifier::removeNotification(const QUuid &id)
{
	if(_notifications.remove(id) > 0)
		updateIcon();
}

void WidgetsNotifier::activated(QSystemTrayIcon::ActivationReason reason)
{
	Q_UNUSED(reason)
	auto dialog = new WidgetsSnoozeDialog(true);
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	connect(dialog, &WidgetsSnoozeDialog::reacted,
			this, &WidgetsNotifier::snoozeAction);
	connect(dialog, &WidgetsSnoozeDialog::aborted,
			this, &WidgetsNotifier::snoozeAborted);

	dialog->addReminders(_notifications.values());
	dialog->open();

	_notifications.clear();
	updateIcon();
}

void WidgetsNotifier::invert()
{
	if(_inverted) {
		_trayIco->setIcon(_normalIcon);
		_inverted = false;
	} else {
		_trayIco->setIcon(_inverseIcon);
		_inverted = true;
	}
}

void WidgetsNotifier::snoozeAction(const QUuid &id, WidgetsSnoozeDialog::Action action, const QDateTime &snoozeTime)
{
	switch (action) {
	case WidgetsSnoozeDialog::CompleteAction:
		emit messageCompleted(id);
		break;
	case WidgetsSnoozeDialog::DefaultSnoozeAction:
		emit messageDismissed(id);
		break;
	case WidgetsSnoozeDialog::SnoozeAction:
		emit messageDelayed(id, snoozeTime);
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

void WidgetsNotifier::snoozeAborted(const QList<Reminder> &reminders)
{
	foreach(auto rem, reminders)
		_notifications.insert(rem.id(), rem);
	updateIcon();
}

void WidgetsNotifier::updateIcon()
{
	if(_notifications.isEmpty()) {
		_blinkTimer->stop();
		_inverted = false;
		_taskbar->setCounter(0);
		_taskbar->setCounterVisible(false);
		_trayIco->setVisible(false);
		_trayIco->setIcon(_normalIcon);
	} else {
		_taskbar->setCounter(_notifications.size());
		if(!_taskbar->counterVisible())
			_taskbar->setCounterVisible(true);

		auto important = false;
		foreach(auto rem, _notifications) {
			if(rem.isImportant()) {
				important = true;
				break;
			}
		}

		if(important) {
			if(!_blinkTimer->isActive())
				_blinkTimer->start();
		}
		if(!_trayIco->isVisible())
			_trayIco->show();
	}
}

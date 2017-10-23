#include "widgetsnotifier.h"

#include <QApplication>
#include <QTimer>

WidgetsNotifier::WidgetsNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_trayIco(new QSystemTrayIcon(QIcon(QStringLiteral(":/icons/tray/main.ico")), this)),
	_taskbar(new QTaskbarControl(new QWidget())), //create with a dummy widget parent
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
}

void WidgetsNotifier::setupEmtpy()
{
	_taskbar->setCounter(0);
	_taskbar->setCounterVisible(false);
	_trayIco->setVisible(false);
}

void WidgetsNotifier::showNotification(const Reminder &reminder)
{
	_notifications.insert(reminder.id(), reminder);

	_taskbar->setCounter(_notifications.size());
	if(!_taskbar->counterVisible())
		_taskbar->setCounterVisible(true);

	_trayIco->show();
	_trayIco->showMessage(tr("Reminder triggered!"),
						  reminder.text(),
						  QSystemTrayIcon::Information,
						  10000);
}

void WidgetsNotifier::removeNotification(const QUuid &id)
{
	if(_notifications.remove(id) > 0) {
		_taskbar->setCounter(_notifications.size());
		if(_notifications.isEmpty()) {
			_taskbar->setCounterVisible(false);
			_trayIco->hide();
		}
	}
}

void WidgetsNotifier::activated(QSystemTrayIcon::ActivationReason reason)
{
	//TODO show proper GUI, for now debug:
	Q_UNIMPLEMENTED();
}

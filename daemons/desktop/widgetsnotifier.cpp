#include "widgetsnotifier.h"

#include <QApplication>
#include <QSettings>
#include <QTimer>
#include <QtMvvmCore/CoreApp>
#include <dialogmaster.h>
#include "traysnoozeviewmodel.h"

WidgetsNotifier::WidgetsNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_normalIcon(QStringLiteral(":/icons/tray/main.ico")),
	_inverseIcon(QStringLiteral(":/icons/tray/inverse.ico")),
	_errorIcon(QStringLiteral(":/icons/tray/error.ico")),
	_trayIco(new QSystemTrayIcon(_normalIcon, this)),
	_trayMenu(new QMenu()),
	_blinkTimer(new QTimer(this))
{
	connect(_trayIco, &QSystemTrayIcon::destroyed,
			_trayMenu, &QMenu::deleteLater);
}

void WidgetsNotifier::showNotification(const Reminder &reminder)
{
	_notifications.insert(reminder.id(), reminder);
	updateIcon();

	_trayIco->showMessage((reminder.isImportant() ?
							  tr("%1 — Important Reminder") :
							  tr("%1 — Reminder"))
						  .arg(QApplication::applicationDisplayName()),
						  reminder.description(),
						  QSystemTrayIcon::Information);
	qCDebug(notifier) << "Showed notification for reminder with id" << reminder.id();
}

void WidgetsNotifier::removeNotification(QUuid id)
{
	if(_notifications.remove(id) > 0) {
		qCDebug(notifier) << "Removed notification for reminder with id" << id;
		updateIcon();
	}
}

void WidgetsNotifier::showErrorMessage(const QString &error)
{
	_lastError = error;
	updateIcon();

	_trayIco->showMessage(tr("%1 — Error").arg(QApplication::applicationDisplayName()),
						  error,
						  QSystemTrayIcon::Critical);
	qCDebug(notifier) << "Showed critical error message";
}

void WidgetsNotifier::cancelAll()
{
	_notifications.clear();
	qCDebug(notifier) << "Removed all active notifications";
	updateIcon();
}

void WidgetsNotifier::qtmvvm_init()
{
	_trayMenu->addAction(tr("Snooze/Complete Reminders"), this, &WidgetsNotifier::trigger);
	_trayMenu->addAction(tr("Dismiss all"), this, &WidgetsNotifier::cancelAll);
	_trayMenu->addSeparator();
	_trayMenu->addAction(tr("Open Syrem"), this, [this](){
		messageActivated();
	});

	_trayIco->setToolTip(QApplication::applicationDisplayName());
	_trayIco->setContextMenu(_trayMenu);
	connect(_trayIco, &QSystemTrayIcon::activated,
			this, &WidgetsNotifier::activated);
	connect(_trayIco, &QSystemTrayIcon::messageClicked,
			this, &WidgetsNotifier::trigger);

	_blinkTimer->setInterval(_settings->gui.notifications.blinkinterval);
	connect(_blinkTimer, &QTimer::timeout,
			this, &WidgetsNotifier::invert);
}

void WidgetsNotifier::activated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
		trigger();
		break;
	case QSystemTrayIcon::MiddleClick:
		cancelAll();
		break;
	default:
		break;
	}
}

void WidgetsNotifier::trigger()
{
	if(!_lastError.isNull()) {
		auto error = _lastError;
		_lastError.clear();
		updateIcon();

		DialogMaster::critical(nullptr, error, tr("An error occured!"));
	} else if(!_notifications.isEmpty()) {
		QtMvvm::CoreApp::show<TraySnoozeViewModel>(TraySnoozeViewModel::showParams(this, _notifications.values()));
		qCDebug(notifier) << "Showing snooze dialog";
	}
}

void WidgetsNotifier::invert()
{
	if(_inverted) {
		_trayIco->setIcon(_normalIcon);
		_inverted = false;
	} else {
		if(_lastError.isNull())
			_trayIco->setIcon(_inverseIcon);
		else
			_trayIco->setIcon(_errorIcon);
		_inverted = true;
	}
}

void WidgetsNotifier::updateIcon()
{
	if(_notifications.isEmpty() && _lastError.isNull()) {
		_blinkTimer->stop();
		_inverted = false;
		_trayIco->setVisible(false);
		_trayIco->setIcon(_normalIcon);
	} else {
		auto important = false;
		if(_lastError.isNull())
			_trayIco->setToolTip(tr("%1 — %n active reminder(s)", "", _notifications.size())
								 .arg(QApplication::applicationDisplayName()));
		else {
			important = true;
			_trayIco->setToolTip(tr("%1 — An error occured!")
								 .arg(QApplication::applicationDisplayName()));
		}

		if(!important) {
			for(const auto &rem : qAsConst(_notifications)) {
				if(rem.isImportant()) {
					important = true;
					break;
				}
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

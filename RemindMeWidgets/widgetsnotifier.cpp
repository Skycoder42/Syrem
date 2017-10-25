#include "widgetsnotifier.h"

#include <QApplication>
#include <QSettings>
#include <QTimer>
#include <dialogmaster.h>

WidgetsNotifier::WidgetsNotifier(QObject *parent) :
	QObject(parent),
	INotifier(),
	_normalIcon(QStringLiteral(":/icons/tray/main.ico")),
	_inverseIcon(QStringLiteral(":/icons/tray/inverse.ico")),
	_errorIcon(QStringLiteral(":/icons/tray/error.ico")),
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

	QSettings settings;
	_blinkTimer->setInterval(settings.value(QStringLiteral("gui/notifications/blinkinterval"), 750).toInt());
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

	_trayIco->showMessage((reminder.isImportant() ?
							  tr("%1 — Important Reminder") :
							  tr("%1 — Reminder"))
						  .arg(QApplication::applicationDisplayName()),
						  reminder.text(),
						  QSystemTrayIcon::Information);
}

void WidgetsNotifier::removeNotification(const QUuid &id)
{
	if(_notifications.remove(id) > 0)
		updateIcon();
}

void WidgetsNotifier::showErrorMessage(const QString &error)
{
	_lastError = error;
	updateIcon();

	_trayIco->showMessage(tr("%1 — Error").arg(QApplication::applicationDisplayName()),
						  error,
						  QSystemTrayIcon::Critical);
}

void WidgetsNotifier::activated(QSystemTrayIcon::ActivationReason reason)
{
	Q_UNUSED(reason)

	if(!_lastError.isNull()) {
		auto error = _lastError;
		_lastError.clear();
		updateIcon();

		DialogMaster::critical(nullptr, error, tr("An error occured!"));
	} else if(!_notifications.isEmpty()) {
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

void WidgetsNotifier::snoozeAction(Reminder reminder, WidgetsSnoozeDialog::Action action, const QDateTime &snoozeTime)
{
	switch (action) {
	case WidgetsSnoozeDialog::CompleteAction:
		emit messageCompleted(reminder);
		break;
	case WidgetsSnoozeDialog::DefaultSnoozeAction:
		emit messageDismissed(reminder);
		break;
	case WidgetsSnoozeDialog::SnoozeAction:
		emit messageDelayed(reminder, snoozeTime);
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
	if(_notifications.isEmpty() && _lastError.isNull()) {
		_blinkTimer->stop();
		_inverted = false;
		_taskbar->setCounter(0);
		_taskbar->setCounterVisible(false);
		_trayIco->setVisible(false);
		_trayIco->setIcon(_normalIcon);
	} else {
		_taskbar->setCounter(_notifications.size());
		auto showCtr = !_notifications.isEmpty();
		if(_taskbar->counterVisible() != showCtr)
			_taskbar->setCounterVisible(showCtr);

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
			foreach(auto rem, _notifications) {
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

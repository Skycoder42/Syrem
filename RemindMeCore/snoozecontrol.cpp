#include "snoozecontrol.h"
#include "snoozetimes.h"
#include <QSettings>

SnoozeControl::SnoozeControl(QObject *parent) :
	Control(parent),
	_reminderId(),
	_description(),
	_snoozeTimes()
{
	QSettings settings;
	_snoozeTimes = settings.value(QStringLiteral("daemon/snooze/times"),
								  QVariant::fromValue<SnoozeTimes>({
									  tr("in 20 minutes"),
									  tr("in 1 hour"),
									  tr("in 3 hours"),
									  tr("tomorrow"),
									  tr("in 1 week on Monday")
								  })).value<SnoozeTimes>();
}

QString SnoozeControl::description() const
{
	return _description;
}

QStringList SnoozeControl::snoozeTimes() const
{
	return _snoozeTimes;
}

void SnoozeControl::show(const QUuid &id)
{
	_reminderId = id;
	Control::show();
}

void SnoozeControl::snooze()
{
	Q_UNIMPLEMENTED();
}

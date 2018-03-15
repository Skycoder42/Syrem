#include "snoozeviewmodel.h"
#include "snoozetimes.h"
#include <QSettings>
#include <QtMvvmCore/Messages>
#include <rep_snoozehelper_replica.h>
#include "remindmeapp.h"

SnoozeViewModel::SnoozeViewModel(QObject *parent) :
	ViewModel(parent),
	_snoozeHelper(coreApp->node()->acquire<SnoozeHelperReplica>()),
	_loaded(false),
	_reminderId(),
	_description(tr("<i>Loading Reminder, please wait…</i>")),
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

	connect(_snoozeHelper, &SnoozeHelperReplica::reminderLoaded,
			this, &SnoozeViewModel::reminderLoaded);
	connect(_snoozeHelper, &SnoozeHelperReplica::reminderError,
			this, &SnoozeViewModel::reminderError);
	connect(_snoozeHelper, &SnoozeHelperReplica::reminderSnoozed,
			this, &SnoozeViewModel::reminderSnoozed);
}

QVariantHash SnoozeViewModel::showParams(const QUuid &id, quint32 versionCode)
{
	return {
		{QStringLiteral("id"), id},
		{QStringLiteral("versionCode"), versionCode}
	};
}

bool SnoozeViewModel::isLoaded() const
{
	return _loaded;
}

QString SnoozeViewModel::description() const
{
	return _description;
}

QStringList SnoozeViewModel::snoozeTimes() const
{
	return _snoozeTimes;
}

QString SnoozeViewModel::expression() const
{
	return _expression;
}

void SnoozeViewModel::snooze()
{
	_snoozeHelper->snoozeReminder(_reminderId, _expression);
}

void SnoozeViewModel::setExpression(QString expression)
{
	if (_expression == expression)
		return;

	_expression = expression;
	emit expressionChanged(_expression);
}

void SnoozeViewModel::onInit(const QVariantHash &params)
{
	_reminderId = params.value(QStringLiteral("id")).toUuid();
	auto versionCode = params.value(QStringLiteral("versionCode")).toUInt();
	if(_snoozeHelper->isInitialized())
		_snoozeHelper->loadReminder(_reminderId, versionCode);
	else {
		connect(_snoozeHelper, &SnoozeHelperReplica::initialized, _snoozeHelper, [this, versionCode](){
			_snoozeHelper->loadReminder(_reminderId, versionCode);
		});
	}
}

void SnoozeViewModel::reminderLoaded(const QUuid &id, const QString &description)
{
	if(id == _reminderId) {
		_description = tr("Choose a snooze time for the reminder:<br/><i>%1</i>").arg(description);
		emit descriptionChanged(description);
		_loaded = true;
		emit loadedChanged(_loaded);
	}
}

void SnoozeViewModel::reminderSnoozed(const QUuid &id)
{
	if(id == _reminderId)
		emit close();
}

void SnoozeViewModel::reminderError(const QUuid &id, const QString &error, bool close)
{
	if(id == _reminderId) {
		if(close)
			emit this->close();
		QtMvvm::critical(tr("Failed to snooze reminder"), error);
	}
}

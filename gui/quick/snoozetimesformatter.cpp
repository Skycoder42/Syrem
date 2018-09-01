#include "snoozetimesformatter.h"
#include <QCoreApplication>
#include "snoozetimes.h"

QString SnoozeTimesFormatter::format(const QString &formatString, const QVariant &value, const QVariantMap &viewProperties) const
{
	return QCoreApplication::translate(qUtf8Printable(viewProperties.value(QStringLiteral("tr_context")).toString()),
									   qUtf8Printable(formatString),
									   "", value.value<SnoozeTimes>().size());
}

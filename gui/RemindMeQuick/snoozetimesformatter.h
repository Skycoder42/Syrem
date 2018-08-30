#ifndef SNOOZETIMESFORMATTER_H
#define SNOOZETIMESFORMATTER_H

#include <QtMvvmQuick/Formatter>

class SnoozeTimesFormatter : public QtMvvm::Formatter
{
public:
	QString format(const QString &formatString, const QVariant &value, const QVariantMap &viewProperties) const override;
};

#endif // SNOOZETIMESFORMATTER_H

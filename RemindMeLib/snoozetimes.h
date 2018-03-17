#ifndef SNOOZETIMES_H
#define SNOOZETIMES_H

#include <QStringList>
#include <QObject>
#include <QVariant>
#include <QDataStream>

#include "remindmelib_global.h"

class REMINDMELIBSHARED_EXPORT SnoozeTimes : public QStringList
{
	Q_GADGET

public:
	inline SnoozeTimes() = default;
	SnoozeTimes(const std::initializer_list<QString> &args);
	template <typename... Args>
	inline SnoozeTimes(Args... args) :
		QStringList(args...)
	{}

	Q_INVOKABLE QVariantList toList() const;
};

REMINDMELIBSHARED_EXPORT QDataStream &operator<<(QDataStream &stream, const SnoozeTimes &times);
REMINDMELIBSHARED_EXPORT QDataStream &operator>>(QDataStream &stream, SnoozeTimes &times);

Q_DECLARE_METATYPE(SnoozeTimes)

#endif // SNOOZETIMES_H

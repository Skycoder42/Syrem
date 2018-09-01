#ifndef SNOOZETIMES_H
#define SNOOZETIMES_H

#include <QStringList>
#include <QObject>
#include <QVariant>
#include <QDataStream>

#include "libsyrem_global.h"

class LIB_SYREM_EXPORT SnoozeTimes : public QStringList
{
	Q_GADGET

public:
	inline SnoozeTimes() = default;
	SnoozeTimes(std::initializer_list<QString> args);
	template <typename... Args>
	inline SnoozeTimes(Args... args) :
		QStringList(args...)
	{}

	Q_INVOKABLE QVariantList toList() const;
};

class LIB_SYREM_EXPORT SnoozeTimesGenerator : public QObject
{
	Q_OBJECT

public:
	explicit SnoozeTimesGenerator(QObject *parent);

	Q_INVOKABLE SnoozeTimes generate(const QVariant &value) const;
};

LIB_SYREM_EXPORT QDataStream &operator<<(QDataStream &stream, const SnoozeTimes &times);
LIB_SYREM_EXPORT QDataStream &operator>>(QDataStream &stream, SnoozeTimes &times);

Q_DECLARE_METATYPE(SnoozeTimes)

#endif // SNOOZETIMES_H

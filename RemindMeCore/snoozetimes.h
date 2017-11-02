#ifndef SNOOZETIMES_H
#define SNOOZETIMES_H

#include <QStringList>
#include <QObject>
#include <QVariant>

class SnoozeTimes : public QStringList
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

	static void setup();
};

Q_DECLARE_METATYPE(SnoozeTimes)

#endif // SNOOZETIMES_H

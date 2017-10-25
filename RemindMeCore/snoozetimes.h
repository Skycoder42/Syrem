#ifndef SNOOZETIMES_H
#define SNOOZETIMES_H

#include <QStringList>
#include <QObject>

class SnoozeTimes : public QStringList
{
public:
	inline SnoozeTimes() = default;
	inline SnoozeTimes(std::initializer_list<QString> args) :
		QStringList(args)
	{}

	template <typename... Args>
	inline SnoozeTimes(Args... args) :
		QStringList(args...)
	{}

	static inline void setup();
};

Q_DECLARE_METATYPE(SnoozeTimes)

inline void SnoozeTimes::setup() {
	qRegisterMetaType<SnoozeTimes>();
	qRegisterMetaTypeStreamOperators<SnoozeTimes>();

	QMetaType::registerConverter<SnoozeTimes, QVariantList>([](const SnoozeTimes &list) -> QVariantList {
		QVariantList l;
		foreach(auto v, list)
			l.append(QVariant::fromValue(v));
		return l;
	});

	QMetaType::registerConverter<QVariantList, SnoozeTimes>([](const QVariantList &list) -> SnoozeTimes {
		SnoozeTimes l;
		foreach(auto v, list)
			l.append(v.toString());
		return l;
	});
}

#endif // SNOOZETIMES_H

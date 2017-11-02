#include "snoozetimes.h"
#include <QDataStream>

SnoozeTimes::SnoozeTimes(std::initializer_list<QString> args) :
	QStringList(args)
{}

QVariantList SnoozeTimes::toList() const
{
	QVariantList l;
	foreach(auto v, *this)
		l.append(QVariant::fromValue(v));
	return l;
}

void SnoozeTimes::setup() {
	qRegisterMetaType<SnoozeTimes>();
	qRegisterMetaTypeStreamOperators<SnoozeTimes>();

	QMetaType::registerConverter<SnoozeTimes, QVariantList>([](const SnoozeTimes &list) -> QVariantList {
		return list.toList();
	});

	QMetaType::registerConverter<QVariantList, SnoozeTimes>([](const QVariantList &list) -> SnoozeTimes {
		SnoozeTimes l;
		foreach(auto v, list)
			l.append(v.toString());
		return l;
	});
}

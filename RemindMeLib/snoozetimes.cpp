#include "snoozetimes.h"
#include <QDataStream>

SnoozeTimes::SnoozeTimes(const std::initializer_list<QString> &args) :
	QStringList(args)
{}

QVariantList SnoozeTimes::toList() const
{
	QVariantList l;
	foreach(auto v, *this)
		l.append(QVariant::fromValue(v));
	return l;
}

QDataStream &operator<<(QDataStream &stream, const SnoozeTimes &times)
{
	stream << static_cast<QStringList>(times);
	return stream;
}

QDataStream &operator>>(QDataStream &stream, SnoozeTimes &times)
{
	stream >> static_cast<QStringList&>(times);
	return stream;
}

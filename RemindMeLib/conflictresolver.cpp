#include "conflictresolver.h"

ConflictResolver::ConflictResolver(QObject *parent) :
	GenericConflictResolver<Reminder>(parent)
{}

//NOTE remove once fixed in upstream
QJsonObject ConflictResolver::resolveConflict(int typeId, const QJsonObject &data1, const QJsonObject &data2) const
{
	if(typeId == qMetaTypeId<Reminder>()) {
		QObject scope;
		const QJsonSerializer *ser = this->defaults().serializer();
		auto d1 = ser->deserialize<Reminder>(data1, &scope);
		auto d2 = ser->deserialize<Reminder>(data2, &scope);
		auto res = resolveConflict(d1, d2, &scope);
		if(!res.id().isNull())
			return ser->serialize<Reminder>(resolveConflict(d1, d2, &scope));
		else
			return QJsonObject();
	} else
		return resolveUnknownConflict(typeId, data1, data2);
}

Reminder ConflictResolver::resolveConflict(Reminder data1, Reminder data2, QObject *parent) const
{
	Q_UNUSED(parent)
	Q_ASSERT_X(data1.id() == data2.id(), Q_FUNC_INFO, "Reminders with different IDs cannot be merged");

	//compare version code
	if(data1.versionCode() > data2.versionCode())
		return data1;
	else if(data2.versionCode() > data1.versionCode())
		return data2;

	//compare snooze times
	if(data1.snooze().isValid()) {
		if(!data2.snooze().isValid())
			return data1;
		else if(data1.snooze() > data2.snooze()) //shorter snooze wins
			return data2;
		else if(data2.snooze() > data1.snooze()) //shorter snooze wins
			return data1;
	} else if(data2.snooze().isValid())
		return data2;

	//compare schedule
	if(data1.schedule()->current() > data2.schedule()->current())
		return data1;
	else if(data2.schedule()->current() > data1.schedule()->current())
		return data2;

	//default case: no clear winner
	Reminder r;
	r.setId(QUuid());
	return r;
}

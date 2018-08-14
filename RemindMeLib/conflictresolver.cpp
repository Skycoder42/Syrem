#include "conflictresolver.h"

ConflictResolver::ConflictResolver(QObject *parent) :
	GenericConflictResolver<Reminder>(parent)
{}

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
	else
		throw NoConflictResultException{};
}

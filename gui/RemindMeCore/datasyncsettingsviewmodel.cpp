#include "datasyncsettingsviewmodel.h"
#include <QDataStream>
#include <QDebug>

DataSyncSettingsViewModel::DataSyncSettingsViewModel(QObject *parent) :
	SettingsViewModel(parent),
	_store(new QtDataSync::DataTypeStore<DataSyncSettingsEntry>(this))
{}

QVariant DataSyncSettingsViewModel::loadValue(const QString &key, const QVariant &defaultValue) const
{
	try {
		QVariant value;
		{
			auto data = _store->load(key).value;
			QDataStream stream(data);
			stream >> value;
		}
		return value;
	} catch (QtDataSync::NoDataException &e) {
		return defaultValue;
	} catch (QException &e) {
		qCritical() << "Failed to load entry" << key << "from datasync settings with error:"
					<< e.what();
		return defaultValue;
	}
}

void DataSyncSettingsViewModel::saveValue(const QString &key, const QVariant &value)
{
	try {
		QByteArray data;
		{
			QDataStream stream(&data, QIODevice::WriteOnly);
			stream << value;
		}
		_store->save({key, data});
	} catch (QException &e) {
		qCritical() << "Failed to save entry" << key << "to datasync settings with error:"
					<< e.what();
	}
}

void DataSyncSettingsViewModel::resetValue(const QString &key)
{
	try {
		_store->remove(key);
	} catch (QException &e) {
		qCritical() << "Failed to remove entry" << key << "from datasync settings with error:"
					<< e.what();
	}
}

#ifndef DATASYNCSETTINGSVIEWMODEL_H
#define DATASYNCSETTINGSVIEWMODEL_H

#include <QObject>
#include <QtMvvmCore/SettingsViewModel>
#include <QtDataSync/DataTypeStore>

struct DataSyncSettingsEntry //copied from qsettingsgenerator
{
	Q_GADGET

public:
	Q_PROPERTY(QString key MEMBER key USER true)
	Q_PROPERTY(QByteArray value MEMBER value)

	QString key;
	QByteArray value;
};

class DataSyncSettingsViewModel : public QtMvvm::SettingsViewModel
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit DataSyncSettingsViewModel(QObject *parent = nullptr);

	QVariant loadValue(const QString &key, const QVariant &defaultValue) const override;
	void saveValue(const QString &key, const QVariant &value) override;
	void resetValue(const QString &key) override;

private:
	QtDataSync::DataTypeStore<DataSyncSettingsEntry> *_store;
};

#endif // DATASYNCSETTINGSVIEWMODEL_H

#ifndef TERMCONVERTER_H
#define TERMCONVERTER_H

#include <QJsonTypeConverter>

class TermConverter : public QJsonTypeConverter
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value, const SerializationHelper *helper) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent, const SerializationHelper *helper) const override;
};

#endif // TERMCONVERTER_H

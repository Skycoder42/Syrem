#include "termconverter.h"
#include "eventexpressionparser.h"
#include <QJsonArray>
#include <QJsonSerializerException>
using namespace Expressions;

bool TermConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == qMetaTypeId<Term>();
}

QList<QJsonValue::Type> TermConverter::jsonTypes() const
{
	return {QJsonValue::Array};
}

QJsonValue TermConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	if(propertyType != qMetaTypeId<Term>())
		throw QJsonSerializationException{"Unsupported property type. Must be Expressions::Term"};

	auto index = 0;
	QJsonArray array;
	for(const auto &subTerm : value.value<Term>()) {
		array.append(helper->serializeSubtype(qMetaTypeId<SubTerm*>(),
											  QVariant::fromValue(subTerm.data()),
											  "[" + QByteArray::number(index++) + "]"));
	}
	return array;
}

QVariant TermConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	Q_UNUSED(parent)
	if(propertyType != qMetaTypeId<Term>())
		throw QJsonDeserializationException{"Unsupported property type. Must be Expressions::Term"};

	auto index = 0;
	auto array = value.toArray();
	Term term;
	term.reserve(array.size());
	for(auto val : array) {
		auto subTerm = helper->deserializeSubtype(qMetaTypeId<SubTerm*>(),
												  val,
												  nullptr,
												  "[" + QByteArray::number(index++) + "]").value<SubTerm*>();
		if(!subTerm)
			throw QJsonDeserializationException{"Value returned from subterm element in json was not a Expressions::SubTerm"};
		subTerm->setParent(nullptr); //to be shure
		term.append(QSharedPointer<SubTerm>{subTerm});
	}

	term.finalize();
	return QVariant::fromValue(term);
}

#include "registry.h"

#include <QDebug>
#include <QMetaClassInfo>

QHash<QByteArray, QObject*> Registry::_instances;
QHash<QByteArray, Factory*> Registry::_factories;

void Registry::registerObject(const char *interface, QObject *object)
{
	auto old = _instances.take(interface);
	if(old)
		old->deleteLater();
	object->setParent(nullptr);
	_instances.insert(interface, object);
}

QObject *Registry::acquireObject(const char *interface)
{
	return _instances.value(interface);
}

void Registry::registerFactory(const char *interface, Factory *factory)
{
	auto old = _factories.take(interface);
	if(old)
		delete old;
	_factories.insert(interface, factory);
}

void Registry::registerFactory(const char *interface, const std::function<QObject *(QObject *)> &factory)
{
	registerFactory(interface, new FunctionFactory(factory));
}

void Registry::registerFactory(const char *interface, const QMetaObject *metaObject)
{
	registerFactory(interface, new MetaFactory(metaObject));
}

QObject *Registry::createObject(const char *interface, QObject *parent)
{
	auto factory = _factories.value(interface);
	if(factory)
		return factory->create(parent);
	else
		return nullptr;
}

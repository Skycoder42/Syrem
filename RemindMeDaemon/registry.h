#ifndef REGISTRY_H
#define REGISTRY_H

#include <QHash>
#include <QObject>
#include <functional>
#include <type_traits>

class Factory
{
public:
	virtual inline ~Factory() = default;
	virtual QObject *create(QObject *parent) const = 0;
};

template <typename TInterface, typename TImplementation>
class GenericFactory : public Factory
{
	static_assert(std::is_base_of<TInterface, TImplementation>::value, "TImplementation must implement TInterface");
	static_assert(std::is_base_of<QObject, TImplementation>::value, "TImplementation must extend QObject");
public:
	virtual TImplementation *createGeneric(QObject *parent) const = 0;
	inline QObject *create(QObject *parent) const final {
		return createGeneric(parent);
	}
};

class Registry : public QObject
{
	Q_OBJECT

public:
	template <typename TInterface, typename TImplementation>
	static void registerClass(TImplementation *implementation);
	template <typename TInterface, typename TImplementation>
	static void registerClass();

	template <typename TInterface>
	static TInterface *aquire();

	template <typename TInterface, typename TImplementation>
	static void registerFactory(GenericFactory<TInterface, TImplementation> *factory);
	template <typename TInterface, typename TFactory>
	static void registerFactory(TFactory *factory);
	template <typename TInterface,  typename TImplementation>
	static void registerFactory(const std::function<TImplementation *(QObject *)> &factory);
	template <typename TInterface, typename TImplementation>
	static void registerFactory();

	template <typename TInterface>
	static TInterface *create(QObject *parent);

	static void registerObject(const char *interface, QObject *object);
	static QObject *aquireObject(const char *interface);

	static void registerFactory(const char *interface, Factory *factory);
	static void registerFactory(const char *interface, const std::function<QObject *(QObject *)> &factory);
	static void registerFactory(const char *interface, const QMetaObject *metaObject);
	static QObject *createObject(const char *interface, QObject *parent);

private:
	class FunctionFactory : public Factory
	{
	public:
		inline FunctionFactory(const std::function<QObject *(QObject *)> &factory) :
			Factory(),
			_fn(factory)
		{}

		inline QObject *create(QObject *parent) const override {
			return _fn(parent);
		}

	private:
		const std::function<QObject *(QObject *)> _fn;
	};

	class MetaFactory : public Factory
	{
	public:
		inline MetaFactory(const QMetaObject *metaObject) :
			Factory(),
			_metaObject(metaObject)
		{}

		inline QObject *create(QObject *parent) const override {
			Q_ASSERT_X(_metaObject->constructorCount() > 0, Q_FUNC_INFO, "you must specify a Q_INVOKABLE constructor!");
			return _metaObject->newInstance(Q_ARG(QObject*, parent));
		}

	private:
		const QMetaObject *_metaObject;
	};

	static QHash<QByteArray, QObject*> _instances;
	static QHash<QByteArray, Factory*> _factories;
};

template<typename TInterface, typename TImplementation>
void Registry::registerClass(TImplementation *implementation)
{
	static_assert(std::is_base_of<TInterface, TImplementation>::value, "TImplementation must implement TInterface");
	static_assert(std::is_base_of<QObject, TImplementation>::value, "TImplementation must extend QObject");
	registerObject(qobject_interface_iid<TInterface*>(), implementation);
}

template<typename TInterface, typename TImplementation>
void Registry::registerClass()
{
	static_assert(std::is_base_of<TInterface, TImplementation>::value, "TImplementation must implement TInterface");
	static_assert(std::is_base_of<QObject, TImplementation>::value, "TImplementation must extend QObject");
	registerObject(qobject_interface_iid<TInterface*>(), new TImplementation());
}

template<typename TInterface>
TInterface *Registry::aquire()
{
	return qobject_cast<TInterface*>(aquireObject(qobject_interface_iid<TInterface*>()));
}

template<typename TInterface, typename TImplementation>
void Registry::registerFactory(GenericFactory<TInterface, TImplementation> *factory)
{
	static_assert(std::is_base_of<TInterface, TImplementation>::value, "TImplementation must implement TInterface");
	static_assert(std::is_base_of<QObject, TImplementation>::value, "TImplementation must extend QObject");
	registerFactory(qobject_interface_iid<TInterface*>(), factory);
}

template<typename TInterface, typename TFactory>
void Registry::registerFactory(TFactory *factory)
{
	static_assert(std::is_base_of<Factory, TFactory>::value, "TFactory must implement Factory");
	registerFactory(qobject_interface_iid<TInterface*>(), factory);
}

template<typename TInterface, typename TImplementation>
void Registry::registerFactory(const std::function<TImplementation *(QObject *)> &factory)
{
	static_assert(std::is_base_of<TInterface, TImplementation>::value, "TImplementation must implement TInterface");
	static_assert(std::is_base_of<QObject, TImplementation>::value, "TImplementation must extend QObject");
	registerFactory(qobject_interface_iid<TInterface*>(), new FunctionFactory([factory](QObject *parent) {
						return factory(parent);
					}));
}

template<typename TInterface, typename TImplementation>
void Registry::registerFactory()
{
	static_assert(std::is_base_of<TInterface, TImplementation>::value, "TImplementation must implement TInterface");
	static_assert(std::is_base_of<QObject, TImplementation>::value, "TImplementation must extend QObject");
	registerFactory(qobject_interface_iid<TInterface*>(), new MetaFactory(&TImplementation::staticMetaObject));
}

template<typename TInterface>
TInterface *Registry::create(QObject *parent)
{
	return qobject_cast<TInterface*>(createObject(qobject_interface_iid<TInterface*>(), parent));
}

#endif // REGISTRY_H

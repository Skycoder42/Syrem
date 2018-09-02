#ifndef SYREMAPP_H
#define SYREMAPP_H

#include <QtMvvmCore/CoreApp>
#include <QtService/ServiceControl>

class SyremApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	explicit SyremApp(QObject *parent = nullptr);

	Q_INVOKABLE bool isCreateOnly() const;

protected:
	void performRegistrations() override;
	int startApp(const QStringList &arguments) override;

private:
#ifndef Q_OS_ANDROID
	QtService::ServiceControl *_serviceControl = nullptr;
#endif
	bool _createOnly = false;

	void createReminderInline(bool important, const QString &description, const QString &when);
};

#undef coreApp
#define coreApp static_cast<SyremApp*>(QtMvvm::CoreApp::instance())

#endif // SYREMAPP_H
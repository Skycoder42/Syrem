#ifndef SYREMAPP_H
#define SYREMAPP_H

#include <QtMvvmCore/CoreApp>
#include <QtService/ServiceControl>

#ifndef Q_OS_ANDROID
#include <QProcess>
#ifndef QT_NO_DEBUG
#define USE_DEBUG_SERVICE
#else
#define USE_RELEASE_SERVICE
#endif
#endif

class SyremApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	explicit SyremApp(QObject *parent = nullptr);

	Q_INVOKABLE bool isCreateOnly() const;
	Q_INVOKABLE void resetIsCreateOnly(); //WORKAROUND see qml

protected:
	void performRegistrations() override;
	int startApp(const QStringList &arguments) override;

private:
#ifdef USE_DEBUG_SERVICE
	QProcess *_serviceControl = nullptr;
#elif defined(USE_SYSTEMD_SERVICE)
	QtService::ServiceControl *_serviceControl = nullptr;
#endif
	bool _createOnly = false;

	void createReminderInline(bool important, const QString &description, const QString &when);
};

#undef coreApp
#define coreApp static_cast<SyremApp*>(QtMvvm::CoreApp::instance())

#endif // SYREMAPP_H

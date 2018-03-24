#ifndef REMINDMEAPP_H
#define REMINDMEAPP_H

#include <QtMvvmCore/CoreApp>

#ifndef Q_OS_ANDROID
#include "daemoncontroller.h"
#endif

class RemindMeApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	explicit RemindMeApp(QObject *parent = nullptr);

	Q_INVOKABLE bool isCreateOnly() const;

protected:
	void performRegistrations() override;
	int startApp(const QStringList &arguments) override;

private Q_SLOTS:
	void createReminderInline(bool important, const QString &description, const QString &when);

private:
#ifndef Q_OS_ANDROID
	DaemonController *_daemon;
#endif
	bool _createOnly;
};

#undef coreApp
#define coreApp static_cast<RemindMeApp*>(QtMvvm::CoreApp::instance())

#endif // REMINDMEAPP_H

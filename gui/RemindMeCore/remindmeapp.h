#ifndef REMINDMEAPP_H
#define REMINDMEAPP_H

#include <QtMvvmCore/CoreApp>

#include "daemoncontroller.h"

class RemindMeApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	explicit RemindMeApp(QObject *parent = nullptr);

protected:
	void performRegistrations() override;
	int startApp(const QStringList &arguments) override;

private Q_SLOTS:
	void createReminderInline(bool important, const QString &description, const QString &when);

private:
	DaemonController *_daemon;
};

#undef coreApp
#define coreApp static_cast<RemindMeApp*>(QtMvvm::CoreApp::instance())

#endif // REMINDMEAPP_H

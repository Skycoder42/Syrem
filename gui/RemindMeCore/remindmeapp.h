#ifndef REMINDMEAPP_H
#define REMINDMEAPP_H

#include <QtMvvmCore/CoreApp>

class RemindMeApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	explicit RemindMeApp(QObject *parent = nullptr);

protected:
	void performRegistrations() override;
	int startApp(const QStringList &arguments) override;
};

#undef coreApp
#define coreApp static_cast<RemindMeApp*>(CoreApp::instance())

#endif // REMINDMEAPP_H

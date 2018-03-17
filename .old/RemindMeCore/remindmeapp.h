#ifndef REMINDMEAPP_H
#define REMINDMEAPP_H

#include <QtMvvmCore/CoreApp>
#include <QRemoteObjectNode>

#include "mainviewmodel.h"

class RemindMeApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	explicit RemindMeApp(QObject *parent = nullptr);

	QRemoteObjectNode *node() const;

	void setupParser(QCommandLineParser &parser) const;

public slots:
	void commandMessage(const QStringList &message);
	void showMainControl();
	void showSnoozeControl(const QUuid &id, quint32 versionCode);

protected:
	void performRegistrations();
	int startApp(const QStringList &arguments) override;

private:
	QRemoteObjectNode *_roNode;

	MainViewModel *_mainControl;

	void createFromCli(const QString &text, const QString &when, bool important);
};

#undef coreApp
#define coreApp static_cast<RemindMeApp*>(QtMvvm::CoreApp::instance())

#endif // REMINDMEAPP_H

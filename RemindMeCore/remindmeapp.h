#ifndef REMINDMEAPP_H
#define REMINDMEAPP_H

#include <coreapp.h>
#include <QRemoteObjectNode>

#include "maincontrol.h"

class RemindMeApp : public CoreApp
{
	Q_OBJECT

public:
	explicit RemindMeApp(QObject *parent = nullptr);

	QRemoteObjectNode *node() const;

public slots:
	void commandMessage(const QStringList &message);

protected:
	void setupParser(QCommandLineParser &parser, bool &allowInvalid) const override;
	bool startApp(const QCommandLineParser &parser) override;

protected slots:
	void aboutToQuit() override;

private:
	QRemoteObjectNode *_roNode;

	MainControl *_mainControl;

	void createFromCli(const QString &text, const QString &when, bool important);
};

#undef coreApp
#define coreApp static_cast<RemindMeApp*>(CoreApp::instance())

#endif // REMINDMEAPP_H

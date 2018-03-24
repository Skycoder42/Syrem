#ifndef DAEMONCONTROLLER_H
#define DAEMONCONTROLLER_H

#include <QObject>
#include <QProcess>

#if (!defined(QT_NO_DEBUG) || !defined(Q_OS_LINUX)
#define NEEDS_QPROCESS
#endif

class DaemonController : public QObject
{
	Q_OBJECT

public:
	explicit DaemonController(QObject *parent = nullptr);

	void ensureStarted();
	void restart();
	void stop();

private:
#ifdef NEEDS_QPROCESS
	QProcess *_process;
#else
	bool runSystemCtl(const QString &command) const;
#endif
};

#endif // DAEMONCONTROLLER_H
